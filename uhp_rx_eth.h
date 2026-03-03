#ifndef TCP_PROTOCOL_DEFINES_H_
#define TCP_PROTOCOL_DEFINES_H_

#include <string>
#include <cstdint>

// ----- ВЕРСИЯ ПРОТОКОЛА -----
#define UHP_VERSION "v2025.1.0"
// ----------------------------

// Трансляция идентификатора (UDP): порт который надо слушать на стороне клиента
#define UDP_ID_BROADCAST_PORT_RECV 42000

// Для сервиса "Обмен командами" (TCP):
#define UHP_PORT_CH1 5050
#define UHP_PORT_CH2 5051 // используется только в ПРМ4
#define UHP_PORT_CH3 5052
#define UHP_PORT_CH4 5053 // используется только в ПРМ4

#define MAX_CTRL_PACKET_SIZE 102400

#pragma pack(push, 1) // включает точный расчет размера структуры по её полям, иначе округляет

namespace ETH_RX_CTRL
{
   /// @brief Перечень ID-команд протокола
   enum COMMAND
   {
      ANSWER_0x0 = 0x0,
      GET_STATUS_0x1 = 0x1,
      SET_FREQ_REQUEST_0x2 = 0x2,
      ADD_PRESET_REQUEST_0x3 = 0x3,
      SET_FREQ_ADD_PRESET_REQUEST_0x4 = 0x4,
      SET_ATT_BITMASK_0x5 = 0x5,
      SET_ANTENNA_0x6 = 0x6,
      SET_MODULATION_0x7 = 0x7,
      SET_SIGNAL_BAND_0x8 = 0x8,
      SET_GENERATOR_SOURCE_0x9 = 0x9,
      SET_SAMPLE_RATE_0xA = 0xA,
      SET_SETTINGS_0xB = 0xB,
      CTRL_IQ_STREAM_NOW_0xC = 0xC,
      CTRL_IQ_STREAM_SYNC_0xD = 0xD,
      STOP_IQ_STREAM_0xE = 0xE,
      DIAGNOSTIC_0xF = 0xF,
      RESET_0x10 = 0x10,
      SET_N_IQ_SLICE_0x11 = 0x11,
      SET_IP_ADDRESS_0x12 = 0x12,
      SET_CONTROL_MODE_0x13 = 0x13,
      SET_PRESET_0x14 = 0x14,
      SET_GENERATOR_MONITORING_0x15 = 0x15,
      SET_GENERATOR_FREQ_ADJUSTMENT_0x16 = 0x16,
      SET_LOG_DESTINATION_0x17 = 0x17,
      SET_ARU_STATE_0x18 = 0x18,
      RESET_PROTECTION_0x19 = 0x19,
      GET_ID_AND_SOFT_VERSION_0x1A = 0x1A,
      GET_ID_AND_SOFT_VERSION_ANS_0x1B = 0x1B,
      SET_IQ_FORMAT_0x1C = 0x1C,
      UPDATE_PRESELECTOR_TEMPERATURE_0x1D = 0x1D,

      ERROR_0xFF = 0xFF,
   };

   /// @brief Доп команды для режиме дебага
   enum COMMAND_DEBUG
   {
      CALIBRATION_0x100 = 0x100,
      // INDICATOR_CLIENT_0x101 = 0x101, // deprecated
      // Команды управления преселектором
      SET_C1_DPF_0x102 = 0x102,
      SET_C2_DPF_0x103 = 0x103,
      SET_C1_TPF_0x104 = 0x104,
      SET_C2_TPF_0x105 = 0x105,
      SET_C3_TPF_0x106 = 0x106,
      SET_BAND_DPF_0x107 = 0x107,
      SET_BAND_TPF_0x108 = 0x108,
      SET_FREQ_PRESELECTOR_0x109 = 0x109,
      SET_FILTER_0x10A = 0x10A,
      SET_IN_ATT_0x10B = 0x10B,
      SET_OUT_ATT_0x10C = 0x10C,
      SET_TEST_GEN_0x10D = 0x10D,
      SET_TEMPERATURE_THRESHOLDS_0x10E = 0x10E,
      GET_PRESELECTOR_TEMPERATURE_0x10F = 0x10F,
      WRITE_PRESELECTOR_MEMORY_0x110 = 0x110,
      READ_PRESELECTOR_MEMORY_0x111 = 0x111,
      SET_PRESELECTOR_AMPLIFIER_0x112 = 0x112,
      SET_RF_IN_LOCK_0x113 = 0x113,
      RESET_PRESELECTOR_PROTECTION_0x114 = 0x114,
      RESET_PRESELECTOR_HARDWARE_0x115 = 0x115,
      INIT_PRESELECTOR_HARDWARE_0x116 = 0x116,
      READ_PRESELECTOR_MEMORY_ANS_0x117 = 0x117,
      GET_PRESELECTOR_TEMPERATURE_ANS_0x118 = 0x118,
      INIT_TEST_ERROR_MSG_0x119 = 0x119,
      GET_DAC_GENERATOR_VALUE_0x11A = 0x11A,
      GET_DAC_GENERATOR_VALUE_ANS_0x11B = 0x11B,
      CHECK_PRESELECTOR_I2C_CHIPS_0x11C = 0x11C,
      CHECK_PRESELECTOR_I2C_CHIPS_ANS_0x11D = 0x11D,
      SET_DPF_FILTER_0x11E = 0x11E,
      SET_TPF_FILTER_0x11F = 0x11F
      // ---------------------------------
   };

   const int global_cmds[] =
      {
         COMMAND::SET_GENERATOR_SOURCE_0x9,
         COMMAND::SET_SETTINGS_0xB,
         COMMAND::DIAGNOSTIC_0xF,
         COMMAND::RESET_0x10,
         COMMAND::SET_IP_ADDRESS_0x12,
         COMMAND::SET_CONTROL_MODE_0x13,
         COMMAND::CTRL_IQ_STREAM_NOW_0xC,  // рассылка произойдет только при переходе из standby->Rx
         COMMAND::CTRL_IQ_STREAM_SYNC_0xD, // рассылка произойдет только при переходе из standby->Rx
         COMMAND::STOP_IQ_STREAM_0xE,      // рассылка произойдет только при переходе из Rx->Standby
         COMMAND::SET_GENERATOR_MONITORING_0x15,
         COMMAND::SET_GENERATOR_FREQ_ADJUSTMENT_0x16,
         COMMAND::SET_LOG_DESTINATION_0x17,
         COMMAND::SET_IQ_FORMAT_0x1C,
         COMMAND_DEBUG::CALIBRATION_0x100
      };

   const int cmd_allow_in_any_ctrl_mode[] =
      {
         COMMAND::GET_STATUS_0x1,
         COMMAND::SET_CONTROL_MODE_0x13,
         COMMAND::CTRL_IQ_STREAM_NOW_0xC,
         COMMAND::STOP_IQ_STREAM_0xE
      };

   /// @brief функция для более удобного отображения значения ID-команд протокола в лог
   inline std::string cmdCOMMANDtoStr(uint16_t val)
   {
      switch (val)
      {
         case COMMAND::ANSWER_0x0:
            return "ANSWER_0x0";
         case COMMAND::GET_STATUS_0x1:
            return "GET_STATUS_0x1";
         case COMMAND::SET_FREQ_REQUEST_0x2:
            return "SET_FREQ_REQUEST_0x2";
         case COMMAND::ADD_PRESET_REQUEST_0x3:
            return "ADD_PRESET_REQUEST_0x3";
         case COMMAND::SET_FREQ_ADD_PRESET_REQUEST_0x4:
            return "SET_FREQ_ADD_PRESET_REQUEST_0x4";
         case COMMAND::SET_ATT_BITMASK_0x5:
            return "SET_ATT_BITMASK_0x5";
         case COMMAND::SET_ANTENNA_0x6:
            return "SET_ANTENNA_0x6";
         case COMMAND::SET_MODULATION_0x7:
            return "SET_MODULATION_0x7";
         case COMMAND::SET_SIGNAL_BAND_0x8:
            return "SET_SIGNAL_BAND_0x8";
         case COMMAND::SET_GENERATOR_SOURCE_0x9:
            return "SET_GENERATOR_SOURCE_0x9";
         case COMMAND::SET_SAMPLE_RATE_0xA:
            return "SET_SAMPLE_RATE_0xA";
         case COMMAND::SET_SETTINGS_0xB:
            return "SET_SETTINGS_0xB";
         case COMMAND::CTRL_IQ_STREAM_NOW_0xC:
            return "CTRL_IQ_STREAM_NOW_0xC";
         case COMMAND::CTRL_IQ_STREAM_SYNC_0xD:
            return "CTRL_IQ_STREAM_SYNC_0xD";
         case COMMAND::STOP_IQ_STREAM_0xE:
            return "STOP_IQ_STREAM_0xE";
         case COMMAND::ERROR_0xFF:
            return "ERROR_0xFF";
         case COMMAND::DIAGNOSTIC_0xF:
            return "DIAGNOSTIC_0xF";
         case COMMAND::RESET_0x10:
            return "RESET_0x10";
         case COMMAND::SET_N_IQ_SLICE_0x11:
            return "SET_N_IQ_SLICE_0x11";
         case COMMAND::SET_IP_ADDRESS_0x12:
            return "SET_IP_ADDRESS_0x12";
         case COMMAND::SET_CONTROL_MODE_0x13:
            return "SET_CONTROL_MODE_0x13";
         case COMMAND::SET_PRESET_0x14:
            return "SET_PRESET_0x14";
         case COMMAND::SET_GENERATOR_MONITORING_0x15:
            return "SET_GENERATOR_MONITORING_0x15";
         case COMMAND::SET_GENERATOR_FREQ_ADJUSTMENT_0x16:
            return "SET_GENERATOR_FREQ_ADJUSTMENT_0x16";
         case COMMAND::SET_LOG_DESTINATION_0x17:
            return "SET_LOG_DESTINATION_0x17";
         case COMMAND::SET_ARU_STATE_0x18:
            return "SET_ARU_STATE_0x18";
         case COMMAND::RESET_PROTECTION_0x19:
            return "RESET_PROTECTION_0x19";
         case COMMAND::GET_ID_AND_SOFT_VERSION_0x1A:
            return "GET_ID_AND_SOFT_VERSION_0x1A";
         case COMMAND::GET_ID_AND_SOFT_VERSION_ANS_0x1B:
            return "GET_ID_AND_SOFT_VERSION_ANS_0x1B";
         case COMMAND::SET_IQ_FORMAT_0x1C:
            return "SET_IQ_FORMAT_0x1C";
         case COMMAND::UPDATE_PRESELECTOR_TEMPERATURE_0x1D:
            return "UPDATE_PRESELECTOR_TEMPERATURE_0x1D";

            // технологические команды
         case COMMAND_DEBUG::CALIBRATION_0x100:
            return "CALIBRATION_0x100";
         case COMMAND_DEBUG::SET_C1_DPF_0x102:
            return "SET_C1_DPF_0x102";
         case COMMAND_DEBUG::SET_C2_DPF_0x103:
            return "SET_C2_DPF_0x103";
         case COMMAND_DEBUG::SET_C1_TPF_0x104:
            return "SET_C1_TPF_0x104";
         case COMMAND_DEBUG::SET_C2_TPF_0x105:
            return "SET_C2_TPF_0x105";
         case COMMAND_DEBUG::SET_C3_TPF_0x106:
            return "SET_C3_TPF_0x106";
         case COMMAND_DEBUG::SET_BAND_DPF_0x107:
            return "SET_BAND_DPF_0x107";
         case COMMAND_DEBUG::SET_BAND_TPF_0x108:
            return "SET_BAND_TPF_0x108";
         case COMMAND_DEBUG::SET_FREQ_PRESELECTOR_0x109:
            return "SET_FREQ_PRESELECTOR_0x109";
         case COMMAND_DEBUG::SET_FILTER_0x10A:
            return "SET_FILTER_0x10A";
         case COMMAND_DEBUG::SET_IN_ATT_0x10B:
            return "SET_IN_ATT_0x10B";
         case COMMAND_DEBUG::SET_OUT_ATT_0x10C:
            return "SET_OUT_ATT_0x10C";
         case COMMAND_DEBUG::SET_TEST_GEN_0x10D:
            return "SET_TEST_GEN_0x10D";
         case COMMAND_DEBUG::SET_TEMPERATURE_THRESHOLDS_0x10E:
            return "SET_TEMPERATURE_THRESHOLDS_0x10E";
         case COMMAND_DEBUG::GET_PRESELECTOR_TEMPERATURE_0x10F:
            return "GET_PRESELECTOR_TEMPERATURE_0x10F";
         case COMMAND_DEBUG::WRITE_PRESELECTOR_MEMORY_0x110:
            return "WRITE_PRESELECTOR_MEMORY_0x110";
         case COMMAND_DEBUG::READ_PRESELECTOR_MEMORY_0x111:
            return "READ_PRESELECTOR_MEMORY_0x111";
         case COMMAND_DEBUG::SET_PRESELECTOR_AMPLIFIER_0x112:
            return "SET_PRESELECTOR_AMPLIFIER_0x112";
         case COMMAND_DEBUG::SET_RF_IN_LOCK_0x113:
            return "SET_RF_IN_LOCK_0x113";
         case COMMAND_DEBUG::RESET_PRESELECTOR_PROTECTION_0x114:
            return "RESET_PRESELECTOR_PROTECTION_0x114";
         case COMMAND_DEBUG::RESET_PRESELECTOR_HARDWARE_0x115:
            return "RESET_PRESELECTOR_HARDWARE_0x115";
         case COMMAND_DEBUG::INIT_PRESELECTOR_HARDWARE_0x116:
            return "INIT_PRESELECTOR_HARDWARE_0x116";
         case COMMAND_DEBUG::READ_PRESELECTOR_MEMORY_ANS_0x117:
            return "READ_PRESELECTOR_MEMORY_ANS_0x117";
         case COMMAND_DEBUG::GET_PRESELECTOR_TEMPERATURE_ANS_0x118:
            return "GET_PRESELECTOR_TEMPERATURE_ANS_0x118";
         case COMMAND_DEBUG::INIT_TEST_ERROR_MSG_0x119:
            return "INIT_TEST_ERROR_MSG_0x119";
         case COMMAND_DEBUG::GET_DAC_GENERATOR_VALUE_0x11A:
            return "READ_DAC_GENERATOR_VALUE_0x11A";
         case COMMAND_DEBUG::GET_DAC_GENERATOR_VALUE_ANS_0x11B:
            return "READ_DAC_GENERATOR_VALUE_ANS_0x11B";
         case COMMAND_DEBUG::CHECK_PRESELECTOR_I2C_CHIPS_0x11C:
            return "CHECK_PRESELECTOR_I2C_CHIPS_0x11C";
         case COMMAND_DEBUG::CHECK_PRESELECTOR_I2C_CHIPS_ANS_0x11D:
            return "CHECK_PRESELECTOR_I2C_CHIPS_ANS_0x11D";
         case COMMAND_DEBUG::SET_DPF_FILTER_0x11E:
            return "SET_DPF_FILTER_0x11E";
         case COMMAND_DEBUG::SET_TPF_FILTER_0x11F:
            return "SET_TPF_FILTER_0x11F";

         default:
            return (std::string("UNKNOWN PARAMETER") + std::to_string(val));
      }
   }

   // ============ БЛОК ОСНОВНЫХ КОМАНД =================

   /// @brief Заголовок запроса в ПРМ
   struct header_req
   {
      uint32_t size;
      uint32_t messid;
      uint16_t cmd_type;
   };

   /// @brief (0x1) Запроса состояния ПРМ
   struct get_status
   {
      header_req head;
   };

   /// @brief (0x2) Настроить ПРМ на частоту:
   struct set_freq
   {
      header_req head;

      uint32_t carrier_freq_Hz;
   };

   /// @brief (0x3) Записать текущую частоту в ЗПЧ:
   struct add_preset
   {
      header_req head;

      uint16_t preset_num;
      uint8_t is_auto_calc_att; // bool
   };

   /// @brief (0x4) Настроить ПРМ на частоту и в случае успеха записать в таблицу ЗПЧ:
   struct set_freq_add_preset
   {
      header_req head;

      uint32_t carrier_freq_Hz;
      uint8_t ant;
      uint32_t att_bitmask;
      uint16_t preset_num;
   };

   /// @brief (0x5) Управление аттенюатором:
   struct set_att_bitmask
   {
      header_req head;

      uint32_t att_bitmask;
   };

   /// @brief (0x6) Выбор антенны:
   struct set_antenna
   {
      header_req head;

      uint8_t ant; // enum ant_t
   };

   /// @brief (0x7) Выбор модуляции:
   struct set_modulation
   {
      header_req head;

      uint8_t modulation; // enum modulation_t
   };

   /// @brief (0x9) Выбор источника опорного генератора:
   struct set_generator_source
   {
      header_req head;

      uint8_t gen_source;
   };

   /// @brief (0xA) Установка частоты дискретизации:
   struct set_sample_rate
   {
      header_req head;

      uint32_t sample_rate_Hz;
   };

   /// @brief (0xB) Общая команда настройки:
   struct set_settings
   {
      header_req head;

      uint8_t ant; // enum ant_t
      uint32_t att_bitmask;
      uint8_t gen_source;
      uint8_t modulation; // enum modulation_t
      uint32_t sample_rate_Hz;
      uint8_t N_IQ_slice;
   };

   /// @brief (0xC) Управление потоком. Старт немедленно. Отменяет ожидание синхросигнала
   struct ctrl_iq_stream_now
   {
      header_req head;

      // dest ip в пакетах потока, если 0, то будет выбран ip
      // из dest_ip tcp сокета управляющего соединением
      uint32_t IP_stream;
      uint16_t port_stream;

      // установить ЗПЧ, если 0xffff, то используется текущая частота
      uint16_t preset_num;
   };

   /// @brief (0xD) Управление потоком. Старт по синхросигналу.
   /// Останавливает поток данных до получения синхросигнала.
   struct ctrl_iq_stream_sync
   {
      header_req head;

      // по фронту или по уровню
      uint8_t sync_type;

      // таймаут ожидания в мс
      uint16_t sync_timeout;

      // dest ip в пакетах потока, если 0, то будет выбран ip
      // из dest_ip tcp сокета управляющего соединения
      uint32_t IP_stream;
      uint16_t port_stream;

      // установить ЗПЧ, если 0xffff, то используется текущая частота
      uint16_t preset_num;
   };

   /// @brief (0xE) Остановить поток данных
   struct stop_iq_stream
   {
      header_req head;
   };

   /// @brief (0xF) Провести диагностику
   struct start_diagnostic
   {
      header_req head;

      // Шаг по колодцам от 1 до 512.
      // Для полной диагностики =1,
      // для частичной диагностики число
      // увеличивается.
      uint16_t wells_step; // enum wells_step_t
   };

   /// @brief (0x10) Перевести ПРМ в состояние standby из любого состояния
   struct reset
   {
      header_req head;
   };

   /// @brief (0x11) Установка количества N_IQ в пакете
   ///        Установка работает для частота дискретизации =48кГц.
   ///        Для остальных ЧД нужно добавлять множитель, относительно 48кГц,
   ///        т.е., если установить N_IQ=256 для ЧД=96кГц она будет равна 512.
   struct set_N_IQ_slice
   {
      header_req head;

      uint8_t N_IQ_slice; // enum N_IQ_slice_t
   };

   /// @brief (0x12) Изменить ip-адрес для удаленного управления
   struct set_ip_address
   {
      header_req head;

      uint32_t IP_addr; // Пример "192.168.1.1" = 3232235777
      uint8_t save_in_memory; // bool
   };

   /// @brief (0x13) Изменить режим контроля ПРМ: ручное или удаленное
   struct set_control_mode
   {
      header_req head;

      uint8_t ctrl_mode; // enum ctrl_mode_t
   };

   /// @brief (0x14) Установить пресет с указанным номером
   struct set_preset
   {
      header_req head;

      uint16_t preset_num;
   };

   /// @brief (0x15) Управление выводом сигнала ОГ на внешний разъём
   struct set_gen_monitoring
   {
      header_req head;

      uint8_t is_gen_monitoring;
   };

   /// @brief (0x16) Управление подстройкой частоты опорного генератора на плате ЦОС
   struct set_gen_freq_adjustment
   {
      header_req head;

      uint16_t adjust_val;
      uint8_t save_in_memory; // bool Сохранять ли значения в память для применения при перезагрузке
   };

   /// @brief (0x17) Установка ip и порта назначения UDP-логов
   struct set_log_destination
   {
      header_req head;

      uint32_t log_destination_ip;
      uint16_t log_destination_port;
   };

   /// @brief (0x18) Установка состояния АРУ (автоматической регулировки усиления)
   struct set_aru_state
   {
      header_req head;

      uint8_t aru_state;
   };

   /// @brief (0x19) Сброс защит ПРМ
   struct reset_protection
   {
      header_req head;

      uint8_t reset_in_presel_protection; // bool
      uint8_t reset_out_presel_protection; // bool
      uint8_t reset_ADC_protection; // bool
   };

   /// @brief (0x1A) Запрос версий софта и ID преселекторов
   struct get_id_and_soft_version
   {
      header_req head;
   };

   /// @brief (0x1C) Изменить формат данных в IQ-потоке.
   ///               Глобальная. Параметр применяется для всех каналов.
   struct set_iq_format
   {
      header_req head;

      uint8_t format; // enum iq_format_t в "uhp_iq_stream.h"
   };

   /// @brief (0x1D) Опросить температурный датчик преселектора и обновить буфер.
   ///               Без опроса в статусе передается последнее считанное значение температуры.
   struct update_preselector_temperature
   {
      header_req head;
   };

   // ================== БЛОК ТЕХНОЛОГИЧЕСКИХ КОМАНД ==================

   /// @brief (0x100) Провести калибровку для диагностики
   struct start_calibration
   {
      header_req head;
   };

   /// @brief (0x102) Преселектор: установка С1 ДПФ
   struct set_c1_dpf
   {
      header_req head;

      uint16_t c1;
   };

   /// @brief 0x103 - Преселектор: установка С2 ДПФ
   struct set_c2_dpf
   {
      header_req head;

      uint16_t c2;
   };

   /// @brief 0x104 - Преселектор: установка С1 ТПФ
   struct set_c1_tpf
   {
      header_req head;

      uint16_t c1;
   };

   /// @brief 0x105 - Преселектор: установка С2 ТПФ
   struct set_c2_tpf
   {
      header_req head;

      uint16_t c2;
   };

   /// @brief 0x106 - Преселектор: установка С3 ТПФ
   struct set_c3_tpf
   {
      header_req head;

      uint16_t c3;
   };

   /// @brief 0x107 - Преселектор: установка диапазона ДПФ
   struct set_band_dpf
   {
      header_req head;

      uint8_t band; // bool
   };

   /// @brief 0x108 - Преселектор: установка диапазона ТПФ
   struct set_band_tpf
   {
      header_req head;

      uint8_t band; // bool
   };

   /// @brief Преселектор: установка частоты
   struct set_freq_preselector
   {
      header_req head;

      double freq;
   };

   /// @brief 0x10A - Преселектор: установка фильтра
   struct set_filter
   {
      header_req head;

      uint16_t c1_dpf;
      uint16_t c2_dpf;
      uint16_t c1_tpf;
      uint16_t c2_tpf;
      uint16_t c3_tpf;
      uint8_t band_dpf; // bool
      uint8_t band_tpf; // bool
   };

   /// @brief 0x10B - Преселектор: установка аттенюаторов на входе ячейки
   struct set_in_att
   {
      header_req head;

      uint8_t db6_cond;  // bool
      uint8_t db12_cond;  // bool
      uint8_t db24_cond; // bool
   };

   /// @brief 0x10C - Преселектор: установка аттенюаторов на выходе ячейки
   struct set_out_att
   {
      header_req head;

      uint8_t db10_cond; // bool
   };

   /// @brief 0x10D - Преселектор: установка тестового генератора
   struct set_test_generator
   {
      header_req head;

      uint16_t freq_kHz;
      uint8_t cond; // bool
   };

   /// @brief 0x10E - Преселектор: установка температурных пределов
   struct set_temperature_thresholds
   {
      header_req head;

      float low_limit_local_sensor;
      float hight_limit_local_sensor;

      float low_limit_remote_sensor;
      float hight_limit_remote_sensor;
   };

   /// @brief ## 0x10F - Преселектор: чтение данных датчиков температуры
   struct get_preselector_temperature
   {
      header_req head;
   };

   /// @brief 0x110 - Преселектор: запись данных в eeprom
   /// @warning Структура данных в eeprom описана
   ///          в статье: http://youtrack.ratex.local:8080/articles/MIMO-A-16
   struct write_preselector_memory
   {
      header_req head;

      uint16_t offset;
      uint16_t length;
      // массив байт char[N], где N == length
   };

   /// @brief 0x111 - Преселектор: чтение данных из eeprom
   struct read_preselector_memory
   {
      header_req head;

      uint16_t offset;
      uint16_t length;
   };

   /// @brief 0x112 - Преселектор: включение/выключение питания МШУ
   struct set_preselector_amplifier
   {
      header_req head;

      uint8_t amp_state; // bool
   };

   /// @brief 0x113 - Преселектор: включение/выключения запирания ВЧ входа
   struct set_RF_in_lock
   {
      header_req head;

      uint8_t RF_in_lock; // bool
   };

   /// @brief 0x114 - Преселектор: сброс защит
   struct reset_preselector_protection
   {
      header_req head;
   };

   /// @brief 0x115 - Преселектор: сброс I2C устройств
   struct reset_preselector_hardware
   {
      header_req head;
   };

   /// @brief 0x116 - Преселектор: принудительная инициализация I2C устройств
   struct init_preselector_hardware
   {
      header_req head;
   };

   /// @brief 0x119 - Вызов инициативной ошибки
   struct init_test_error_msg
   {
      header_req head;
   };

   /// @brief 0x11A - Чтения значения напрямую из DAC, управляющего подстройкой генератора
   struct get_dac_generator_value
   {
      header_req head;
   };

   /// @brief Перечень i2c-чипов преселекторов. Используется и в команде и в ответе
   struct preselector_i2c_chips
   {
      uint8_t mux;

      uint8_t port_expander_addr0_ch0;
      uint8_t port_expander_addr1_ch0;
      uint8_t port_expander_addr2_ch0;
      uint8_t port_expander_addr3_ch0;

      uint8_t port_expander_addr0_ch1;
      uint8_t port_expander_addr1_ch1;
      uint8_t port_expander_addr2_ch1;
      uint8_t port_expander_addr3_ch1;

      uint8_t port_expander_addr0_ch2;
      uint8_t port_expander_addr1_ch2;
      uint8_t port_expander_addr2_ch2;
      uint8_t port_expander_addr3_ch2;

      uint8_t temp_sensor;
      uint8_t eeprom;
      uint8_t generator;
   };

   /// @brief 0x11C - Преселектор: Запрос состояния i2с-чипов преселектора
   struct check_preselector_i2c_chips
   {
      header_req head;

      preselector_i2c_chips chips;
   };

   /// @brief 0x11E - Преселектор: Установка фильтра ДПФ
   struct set_dpf_filter
   {
      header_req head;

      uint16_t c1_dpf;
      uint16_t c2_dpf;
      uint8_t band_dpf; // bool
   };

   /// @brief 0x11F - Преселектор: Установка фильтра ТПФ
   struct set_tpf_filter
   {
      header_req head;

      uint16_t c1_tpf;
      uint16_t c2_tpf;
      uint16_t c3_tpf;
      uint8_t band_tpf; // bool
   };

   // ================== БЛОК ОТВЕТОВ ОТ ПРМ ==================

   /// @brief Заголовок ответа от ПРМ
   struct header_ans
   {
      uint32_t size;
      uint32_t messid;
      uint16_t cmd_type;
      uint16_t cmd_ack_type;
      uint16_t cmd_complete; // enum cmd_complete_t
   };

   /// @brief Данные для ответа состояния ПРМ
   struct status_data
   {
      uint8_t ant;              // enum ant_t *RXTX
      uint32_t sample_rate_Hz;  // частота дискретизации *RXTX
      uint32_t carrier_freq_Hz; // несущая частота *RXTX
      uint16_t preset_num;      // номер ЗПЧ, если не выбрано, то 0xffff *RXTX
      uint32_t att_bitmask;     // битовая маска включенных ступеней аттенюации *RXTX
      float att_dB;             // общее ослабление сигнала в дБ *RXTX

      uint8_t xcvr_state;  // enum xsvr_state_t, состояния машины состояний *RXTX
      uint8_t modulation;  // enum modulation_t, тип модуляции *RXTX
      uint8_t ctrl;        // enum ctrl_mode_t, управление удаленное, ручное *RXTX
      uint8_t sync_ctrl;   // enum sync_ctrl_mode_t,  срабатывание по фронту или по ПСП Баркера *RXTX
      uint8_t sync_status; // enum sync_status_t, статус синхросигнала: ожидание, получен, таймаут, не используется *RXTX
      uint8_t gen_source;  // enum gen_t, опорный генератор внутренний или внешний
      uint8_t debug_mode;  // enum debug_mode_t, включен или нет режим дебага

      float adc_temp_C;         // температура ячейки ЦОС, цельсии
      float preselector_temp_C; // температура преселектора 0 канала, цельсии

      uint8_t stream_status;    // enum stream_status_t  поток отдается или нет *RX *TX
      uint32_t stream_IP;       // dst ip в UDP-пакетах потока *RX
      uint16_t stream_src_port; // src port в UDP-пакетах потока *RX
      uint16_t stream_dst_port; // dst port в UDP-пакетах потока *RX *TX

      uint8_t test_low_sens_state; // enum test_low_sens_state_t *RX

      uint32_t hardware_errors_bitmask; // битовая маска, enum hardware_error_t *RXTX
      uint32_t software_errors_bitmask; // битовая маска, enum software_error_t *RXTX

      uint32_t remote_ctrl_IP;       // ip USB-ETH для ручного управления *RX
      uint32_t manual_ctrl_IP;       // ip ETH для удаленного управления *RX

      uint8_t gen_monitoring;        // вкл/выкл вывода сигнала ОГ на разъём ПРМ
      uint16_t gen_freq_adjustment;  // отстройка частоты ОГ в условных единицах

      uint8_t aru_state;             // статус работы АРУ в канале
   };

   /// @brief Структура состояния ПРМ
   struct status
   {
      header_ans head;
      status_data data;
   };

   /// @brief 0x1B - ОТВЕТ на 0x1B - Версии софта и ID преселекторов
   struct get_id_and_soft_version_ans
   {
      uint16_t linux_ver_s;
      uint16_t elf_ver_s;
      uint16_t fpga_ver_s;
      uint16_t UHP_ver_s;
      uint16_t station_name_s;
      uint16_t presel1_year;
      uint16_t presel1_num;
      uint16_t presel2_year;
      uint16_t presel2_num;

      // строка linux_ver и размером linux_ver_s
      // строка elf_ver и размером elf_ver_s
      // строка fpga_ver и размером fpga_ver_s
      // строка UHP_ver и размером UHP_ver_s
      // строка station_name и размером station_name_s
      // !!! в конце передается null-terminated символ "/0"
   };

   /// @brief Данные для отправки сообщения об ошибке
   struct error_data
   {
      // битовая маска, enum hardware_error_t
      uint32_t hardware_errors_bitmask;
      // битовая маска, enum software_error_t
      uint32_t software_errors_bitmask;
      uint16_t msg_size;
      // + По TCP отправляется строка char[msg_size]
   };

   /// @brief Структура ошибки состояния ПРМ
   struct error_state
   {
      header_ans head;
      error_data data;
      // + поле строки размера [head.size - sizeof(header_ans) - sizeof(error_data)]
   };

   // ================== БЛОК ОТВЕТОВ ОТ ПРМ НА ТЕХНОЛОГИЧЕСКИЕ КОМАНДЫ ==================

   /// @brief 0x117 - ОТВЕТ на 0x111 - Преселектор: чтение данных из eeprom
   struct read_preselector_memory_ans
   {
      uint16_t offset;
      uint16_t length;
      // массив байт из eeprom char[length]
   };

   /// @brief 0x118 - ОТВЕТ на 0x10F - Преселектор: чтение данных датчиков температуры
   struct get_preselector_temperature_ans
   {
      float local_temp;
      float remote_temp;
   };

   /// @brief 0x11B - ОТВЕТ на 0x11A - Данные DAC установки частоты ОГ
   struct get_dac_generator_value_ans
   {
      uint16_t dac_value;
   };

   // ================== UNION для парсинга ответов вместе с заголовком ==================

   /// @brief Структура c UNION содержащая все типы сообщений от ПРМ
   struct rx_answer
   {
      ETH_RX_CTRL::header_ans header;
      union ARGS
      {
         ETH_RX_CTRL::status_data state;
         ETH_RX_CTRL::error_data error_msg;
         ETH_RX_CTRL::get_id_and_soft_version_ans id_and_soft_version_msg;
         ETH_RX_CTRL::get_preselector_temperature_ans temp_presel_msg;
         ETH_RX_CTRL::read_preselector_memory_ans memory_presel_msg;
         ETH_RX_CTRL::preselector_i2c_chips chips_presel_msg;
      } arg;
   };

   // ================== БЛОК ВОЗМОЖНЫХ ЗНАЧЕНИЙ ПОЛЕЙ ==================

   /// @brief Дефолтные значения messid
   enum messid_t : uint32_t
   {
      messid_default = 0,
   };

   /// @brief Дефолтные значения cmd_ack_type
   enum cmd_ack_type_t : uint16_t
   {
      cmd_ack_type_default = 0,
   };

   /// @brief Возможные значения cmd_complete
   enum cmd_complete_t : uint16_t
   {
      good = 0,
      wrong_param = 1,
      error = 2,
      wrong_command = 3,
      diagnostic_good = 4,
      wrong_ctrl_mode = 5,
      diagnostic_failed = 6,
      max_cmd_complete_t
   };

   /// @brief Возможные значения ant
   enum ant_t : uint8_t
   {
      ant_internal = 0,
      ant_external = 1,
      max_ant_t
   };

   /// @brief Возможные значения sample_rate
   enum sample_rate : uint32_t
   {
      sr_48_kHz = 48000,
      sr_96_kHz = 96000,
      sr_192_kHz = 192000,
      sr_384_kHz = 384000,
      bad_sr = 0
   };

   /// @brief Возможные значения freq
   enum freq_t : uint32_t
   {
      min_freq = 3000000,
      max_freq = 30000000,
   };

   /// @brief Возможные значения preset
   enum preset_t : uint16_t
   {
      min_preset = 1,
      max_preset = 1000,
      pres_default = 0xffff
   };

   /// @brief значимые биты в маске аттенюаторов
   ///        (итоговое значение может быть сочетанием битов -
   ///        включено несколько аттенюаторов)
   enum att_bitmask_t : uint32_t
   {
      att_off = 0x0,
      att_in_6dB = 0x1,
      att_in_12dB = 0x2,
      att_in_24dB = 0x4,
      att_out_10dB = 0x8,
      max_att_bitmask_t = 0xF
   };

   /// @brief Возможные значения xsvr_state
   enum xcvr_state_t : uint8_t
   {
      init = 0,
      standby = 1,
      diagnostic = 2,
      add_preset_state = 3,
      waiting_for_FAR_SYNC = 4,
      rx = 5,
      count_state_t
   };

   /// @brief Возможные значения modulation
   /// @warning Реализована пока только `mod_IQ`
   enum modulation_t : uint8_t
   {
      mod_IQ = 0,
      mod_J3E_LSB = 1,
      mod_J3E_USB = 2,
      mod_A3E = 3,
      mod_F3E = 4,
      mod_off = 5,
      max_modulation_t
   };

   /// @brief Возможные значения ctrl
   enum ctrl_mode_t : uint8_t
   {
      ctrl_manual = 0,
      ctrl_remote = 1,
      max_ctrl_mode_t
   };

   /// @brief Возможные значения sync_ctrl
   enum sync_type_t : uint8_t
   {
      sync_front = 0,
      sync_barker = 1,
      sync_barker_with_line_check = 2,
      max_sync_type_t
   };

   /// @brief Возможные значения sync_status
   enum sync_status_t : uint8_t
   {
      sync_st_unused = 0,   // не используется
      sync_st_waiting = 1,  // ожидание
      sync_st_accepted = 2, // получен вовремя
      sync_st_timeout = 3,  // время ожидания истекло
      max_sync_status_t
   };

   /// @brief Возможные значения gen_source
   enum gen_source_t : uint8_t
   {
      internal = 0,
      external = 1,
      max_gen_source_t
   };

   /// @brief Возможные значения gen_monitoring
   enum gen_monitoring_t : uint8_t
   {
      gen_disable = 0,
      gen_enable  = 1,
      max_gen_monitoring_t
   };

   /// @brief Возможные значения gen_freq_adjustment
   enum gen_freq_adjustment_t : uint16_t
   {
      min_gen_freq_adjustment = 0,
      max_gen_freq_adjustment = 65535 // uint16
   };

   /// @brief Возможные значения gen_monitoring
   enum aru_state_t : uint8_t
   {
      aru_disable = 0,
      aru_enable  = 1,
      max_aru_state_t
   };

   // @brief Возможные значения поля debug_mode, режима дебага
   enum debug_mode_t : uint8_t
   {
      mode_off = 0,
      mode_on = 1,
      max_debug_mode_t
   };

   /// @brief Возможные значения test_low_sens_state
   enum test_low_sens_state_t : uint8_t
   {
      test_low_sens_off = 0,
      test_low_sens_on = 1,
      max_test_low_sens_state_t
   };

   /// @brief Возможные значения stream_status
   enum stream_status_t : uint8_t
   {
      stream_st_off = 0,
      stream_st_on = 1,
      max_stream_status_t
   };

   /// @brief Диапазон возможных значений для шага в диагностике
   enum wells_step_t : uint16_t
   {
      min_wells_step = 1, // соответствует полной диагностике
      max_wells_step = 512,
      max_wells_step_t
   };

   /// @brief Диапазон возможных значений для N_IQ в пакете на частота дискретизации =48кГц
   enum N_IQ_slice_t : uint8_t
   {
      N_IQ_64 = 0,  // скорость формирования пакета: 1,(3) мс
      N_IQ_128 = 1, // скорость формирования пакета: 2,(6) мс
      N_IQ_256 = 2, // скорость формирования пакета: 5,(3) мс
      N_IQ_512 = 3, // скорость формирования пакета: 10,(6) мс
      max_N_IQ_t
   };

   /// @brief Возможные значения hardware_error
   enum hardware_error_t : uint32_t
   {
      err_hw_noRF_CLK              = 0x00000001, // отсутствует клок опорного генератора.
      err_hw_sync_timeout          = 0x00000002, // синхросигнал не поступил в отведенный таймаут
      err_hw_sync_line_fail        = 0x00000004, // ошибка с линией синхронного запуска
      err_hw_eth_usb_not_connected = 0x00000008, // Не подключена внешняя сетевая USB карта

      // Ошибки в канале
      err_hw_presel_protect_in  = 0x00010000, // сработало входная защита преселектора
      err_hw_presel_protect_out = 0x00020000, // сработала вЫходная защита преселектора
      err_hw_presel_temp_hight  = 0x00040000, // температура преселектора выше нормы
      err_hw_presel_temp_low    = 0x00080000, // температура преселектора ниже нормы
      err_hw_presel_no_answer   = 0x00100000, // преселектор не отвечает по i2c-шине
      err_hw_presel_bad_att     = 0x00200000, // у преселектора есть нерабочие аттенюаторы
      err_hw_presel_bad_wells   = 0x00400000, // данные ВЧ канала измеренные данные вышли из коридора +-3дБ
      err_hw_ADC_overload       = 0x00800000, // перегрузка по входному уровню сигнала
   };

   constexpr uint32_t err_hw_check_in_diagnostic =
      hardware_error_t::err_hw_eth_usb_not_connected
      | hardware_error_t::err_hw_presel_no_answer
      | hardware_error_t::err_hw_presel_bad_att
      | hardware_error_t::err_hw_presel_bad_wells;

   /// @brief Возможные значения software_error
   enum software_error_t : uint32_t
   {
      // Ошибки всего устройства
      err_sw_FPGA_IQ_FIFO_overflow  = 0x00000001, // переполнен буфер IQ
      err_sw_FPGA_IQ_FIFO_underflow = 0x00000002, // буферу IQ не хватает данных

      // Ошибки в канале
      err_sw_failed_send_UDP_IQ                  = 0x00010000, // ошибка отправки IQ на запрошенный UDP
      err_sw_presel_wrong_eeprom_crc32           = 0x00020000, // ошибка проверки CRC32 в данных eeprom
      err_sw_RF_path_calibration                 = 0x00040000, // ошибка чтения данных калибровки ВЧ канала
      err_sw_interchannel_FAR_SYNC_ctrl_conflict = 0x00080000, // получен повторный запрос на ожидание синхронного запуска
      err_sw_presel_wrong_csv_data               = 0x00100000, // ошибка чтения резервных данных фильтров для преселектора из .csv в линуксе
      err_sw_other_error                         = 0x80000000, // используется при вызове ошибки командой протокола, либо во всех остальных недокументированных случае
   };

   constexpr uint32_t err_sw_check_in_diagnostic =
      software_error_t::err_sw_RF_path_calibration;

   // ================== БЛОК ВОЗМОЖНЫХ ЗНАЧЕНИЙ ПОЛЕЙ В ТЕХНОЛОГИЧЕСКИХ КОМАНДАХ ==================

   /// @brief Преселектор: возможные значения установки конденсаторов фильтра
   enum set_cap_t : uint16_t
   {
      min_cap = 0,    // минимальное значение
      max_cap = 4095, // максимальное значение
   };

   /// @brief Преселектор: возможное значения установки диапазона фильтра
   enum set_band_t : uint8_t
   {
      min_band = 0, // минимальное значение
      max_band = 1, // максимальное значение
   };

   const double min_preselector_freq = 3e6; // минимальное значение частоты
   const double max_preselector_freq = 3e7; // максимальное значение частоты

   /// @brief Преселектор: возможные значения вкл/выкл аттенюаторов
   enum set_att_preselector_t : uint8_t
   {
      att_preselector_off = 0,
      att_preselector_on = 1,
   };

   /// @brief Преселектор: частота тестового генератора в кГц
   enum set_freq_test_generator_t : uint16_t
   {
      min_freq_kHz = 3000,  // минимальное значение
      max_freq_kHz = 30000, // максимальное значение
   };

   /// @brief Преселектор: возможные значения вкл/выкл тестового генератора
   enum test_gen_state_t : uint8_t
   {
      test_gen_off = 0,
      test_gen_on = 1,
      max_test_gen_state_t
   };

   const double min_sensor_temp = -40.0; // минимальное значение
   const double max_sensor_temp = 125.0; // максимальное значение

   /// @brief Преселектор: возможные значения объема памяти eeprom
   enum eeprom_size_t : uint16_t
   {
      min_eeprom_size = 0,
      max_eeprom_size = 8192
   };

   /// @brief Преселектор: возможные значения вкл/выкл усилителя
   enum set_preselector_amplifier_t : uint8_t
   {
      amp_off = 0,
      amp_on = 1
   };

   /// @brief Преселектор: возможные значения вкл/выкл запирания ВЧ входа
   enum set_RF_in_lock_t : uint8_t
   {
      RF_in_lock_off = 0,
      RF_in_lock_on = 1
   };

   enum preselector_i2c_chip_t : uint8_t 
   { 
      // Возможные поля в команде
      dont_check = 0,
      check = 1,

      // Возможные поля в ответе
      check_success = 2,
      check_fail = 3,

      max_preselector_i2c_chip_t
   };

} // end namespace ETH_RX_CTRL

// ================== БЛОК ТРАНСЛЯЦИИ ИДЕНТИФИКАТОРА ПО UDP ==================

namespace UDP_BROADCAST
{
   /// @brief пакет UDP трансляции идентификатора ПРМ
   struct IdPacket
   {
      uint8_t id;
      uint16_t modification;
      uint16_t tcp_ctrl_port;
      uint8_t name_size;
      // передается по UDP - char[name_size]
      uint8_t firmware_size;
      // передается по UDP - char[firmware_size]
   };

} // end namespace UDP_BROADCAST

#pragma pack(pop)

#endif // TCP_PROTOCOL_DEFINES_H_
