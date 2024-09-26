#include "ec-server/v2/utils.h"
#include "ec-server/wheel/utils.h"



int InitRuntimeFrom(char* filename){


    char *source = NULL;

    FILE *fp = fopen(filename, "r");

    // 동적 할당 하고... config.json파일 읽고...
    if (fp != NULL) {

        if (fseek(fp, 0L, SEEK_END) == 0) {
    
            long bufsize = ftell(fp);

            if (bufsize == -1) {

                printf("failed to read config.json\n");

                return -1;
            }

        
            source = malloc(sizeof(char) * (bufsize + 1));

            if (fseek(fp, 0L, SEEK_SET) != 0) { 

                printf("failed to read config.json, unexpected EOF\n");

                return -2;

            }

        
            size_t new_len = fread(source, sizeof(char), bufsize, fp);
            if ( ferror( fp ) != 0 ) {
                
                printf("error reading config.json\n");

                fclose(fp);

                free(source);

                return -3;

            } else {

                source[new_len++] = '\0'; 
            
            }
        }
        fclose(fp);
    }




    cJSON *conf_json_root = cJSON_Parse(source);

    if (conf_json_root == NULL){


        printf("error parsing config.json\n");

        free(source);

        return -4;

    }

    cJSON *conf_json = cJSON_GetObjectItemCaseSensitive(conf_json_root, "drive");

    cJSON *drivers_num = cJSON_GetObjectItemCaseSensitive(conf_json, "drivers_num");



    int tmp_drivers_num = drivers_num->valueint;

    if (tmp_drivers_num > MAX_DRIVERS_NUM){

        printf("invalid driver num: too many: %d\n", tmp_drivers_num);


        return -5;

    }


    if (tmp_drivers_num < 1){

        printf("invalid driver num: too few: %d\n", tmp_drivers_num);

        return -6;

    }

    g_num_of_slaves = tmp_drivers_num;

    int alloc_result = AllocRuntime();

    if(alloc_result < 0){

        printf("failed to allocate runtime mem\n");

        return alloc_result;
    }



    cJSON *drivers = cJSON_GetObjectItemCaseSensitive(conf_json, "drivers");

    for (int i = 0; i < g_num_of_slaves; i++){

        int homing_offset_value = 0;

        int start_offset_value = 0;

        cJSON *driver = cJSON_GetArrayItem(drivers, i);

        cJSON *driver_id = cJSON_GetObjectItemCaseSensitive(driver, "id");

        if(driver_id->valueint != i){

            printf("invalid driver id sequence: at %d: got: %d\n", i, driver_id->valueint);

            return -7;

        }

        cJSON *homing_offset = cJSON_GetObjectItemCaseSensitive(driver, "homing_offset");

        homing_offset_value = homing_offset->valueint;

        arr_home_sensor_position[i] = homing_offset_value;

        cJSON *start_offset = cJSON_GetObjectItemCaseSensitive(driver, "start_offset");

        start_offset_value = start_offset->valueint;    

        arr_start_offset[i] = start_offset_value;


    }


    cJSON *if_name = cJSON_GetObjectItemCaseSensitive(conf_json, "if_name");

    if(strlen(if_name->valuestring) > MAX_IFNAME){

        printf("if_name too long \n");

        return -8;

    }

    strcpy(main_if_name, if_name->valuestring);


    cJSON *homing_margin_degrees = cJSON_GetObjectItemCaseSensitive(conf_json, "homing_margin_degrees");

    HOMING_MARGIN_DEGREES = homing_margin_degrees->valueint;

    cJSON *velocity_to_start_offset = cJSON_GetObjectItemCaseSensitive(conf_json, "velocity_to_start_offset");

    VELOCITY_TO_START_OFFSET = velocity_to_start_offset->valueint;

    cJSON *move_fail_threshold = cJSON_GetObjectItemCaseSensitive(conf_json, "move_fail_threshold");

    MOVE_FAIL_THRESHOLD = move_fail_threshold->valueint;


    cJSON *wkc_fail_threshold = cJSON_GetObjectItemCaseSensitive(conf_json, "working_counter_fail_threshold");

    WORKING_COUNTER_FAIL_THRESHOLD = wkc_fail_threshold->valueint;

    cJSON *ecount_threshold = cJSON_GetObjectItemCaseSensitive(conf_json, "error_count_threshold");

    ERROR_COUNT_THRESHOLD = ecount_threshold->valueint;



    cJSON *debug_mode_ = cJSON_GetObjectItemCaseSensitive(conf_json, "debug_mode");

    debug_mode = debug_mode_->valueint;



    cJSON *homing_at_start_ = cJSON_GetObjectItemCaseSensitive(conf_json, "homing_at_start");

    int homing_at_start = homing_at_start_->valueint;

    if(homing_at_start == 1){

        _PHASE_ = ECAT2_HOMING;

        phase_toggle = 0;

    } else {

        _PHASE_ = ECAT2_MOVING;

        phase_toggle = 1;
    }


    printf("successfully initiated runtime from config json\n");

    int wheel_init_result = InitWheelRuntimeFromObject(conf_json_root);

    if(wheel_init_result < 0){

        return -10;

    }

    free(source);

    return 0;
}


int AllocRuntime(){


    arr_home_sensor_position = (int*)malloc(g_num_of_slaves * sizeof(int));

    memset(arr_home_sensor_position, 0 , g_num_of_slaves * sizeof(int));

    arr_start_offset = (int*)malloc(g_num_of_slaves * sizeof(int));

    memset(arr_start_offset, 0 , g_num_of_slaves * sizeof(int));

    motor_rxpdos = (servo_rxpdo_t **)malloc(g_num_of_slaves * sizeof(servo_rxpdo_t*));

    motor_txpdos = (servo_txpdo_t **)malloc(g_num_of_slaves * sizeof(servo_txpdo_t*));



    arr_initial_positions = (int*)malloc(g_num_of_slaves * sizeof(int));

    memset(arr_initial_positions, 0 , g_num_of_slaves * sizeof(int));

    arr_has_moved_to_start_offset = (int*)malloc(g_num_of_slaves * sizeof(int));

    memset(arr_has_moved_to_start_offset, 0 , g_num_of_slaves * sizeof(int));

    arr_is_homing_done = (int*)malloc(g_num_of_slaves * sizeof(int));

    memset(arr_is_homing_done, 0 , g_num_of_slaves * sizeof(int));

    arr_move_fail_count = (int*)malloc(g_num_of_slaves * sizeof(int));

    memset(arr_move_fail_count, 0 , g_num_of_slaves * sizeof(int));

    return 0;

}


void FreeRuntime(){


   do {

      if(arr_home_sensor_position == NULL) {

         break;
      }

      free(arr_home_sensor_position);

   } while (0);

   do {

      if(arr_start_offset == NULL) {

         break;
      }

      free(arr_start_offset);

   } while (0);


   do {

      if(motor_rxpdos == NULL) {

         break;
      }

      free(motor_rxpdos);

   } while (0);


   do {

      if(motor_txpdos == NULL){

         break;

      }


      free(motor_txpdos);

   } while(0);


   do {

      if(arr_initial_positions == NULL) {

         break;
      }

      free(arr_initial_positions);

   } while (0);


   do {

      if(arr_has_moved_to_start_offset == NULL) {

         break;
      }

      free(arr_has_moved_to_start_offset);

   } while (0);



   do {

      if(arr_is_homing_done == NULL) {

         break;
      }

      free(arr_is_homing_done);

   } while (0);



   do {

      if(arr_move_fail_count == NULL) {

         break;
      }

      free(arr_move_fail_count);

   } while (0);



}