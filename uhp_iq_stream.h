#ifndef UDP_IQ_DEFINES_H
#define UDP_IQ_DEFINES_H

#include <cstdint>

#define UDP_IQ_HEADER_STRING    "hf"
#define UDP_IQ_HEADER_STRING_N  2
#define SIZEOF_INT24 3


#pragma pack(push,1)

namespace UDP_IQ
{
    /// @brief Описание флагов в поле header_t.flags (16 бит)
    struct header_flags 
   {
      uint8_t ADC_overload : 1; // переполнение АЦП, ~30 дБ.
      uint8_t presel_protected : 1;
      uint8_t att_6db_on  : 1; // вкл/выкл атт =6дб
      uint8_t att_12db_on : 1; // вкл/выкл атт =12дб
      uint8_t att_24db_on : 1; // вкл/выкл атт =24дб
      uint8_t att_10db_on : 1; // вкл/выкл атт =10дб (после МШУ преселектора)
      uint8_t flag6  : 1;
      uint8_t flag7  : 1;
      uint8_t flag8  : 1;
      uint8_t flag9  : 1;
      uint8_t flag10 : 1;
      uint8_t flag11 : 1;
      uint8_t flag12 : 1;
      uint8_t flag13 : 1;
      uint8_t flag14 : 1;
      uint8_t flag15 : 1;
   };

   /// @brief заголовок IQ пакета
   struct header_t
   {
      char mas_str[UDP_IQ_HEADER_STRING_N];
      uint8_t iq_format; // enum iq_format_t
      uint16_t ant_mask;
      uint8_t seqnum;
      uint32_t sample_rate_Hz;
      uint32_t n_samples;
      header_flags flags; 
      uint8_t ADC_avg_power_dBFS; // временно также передается peak_power
      uint8_t ADC_peak_power_dBFS;
   };

   /// @brief Описывает формат данных в пакетах IQ, поле header_t.iq_format
   enum iq_format_t : uint8_t
   {
      iq_int16           = 0x01, // пока не поддерживается
      iq_int32           = 0x02, // пока не поддерживается
      iq_float32         = 0x03, // пока не поддерживается
      iq_complex_int16   = 0x81, // пока не поддерживается
      iq_complex_int32   = 0x82, 
      iq_complex_float32 = 0x83, // пока не поддерживается
      iq_complex_int24   = 0x84  
   };

   /// @brief соответствует iq_format_t::iq_int16
   struct cint16_t
   {
      int16_t re;
      int16_t im;
   };

   /// @brief соответствует iq_format_t::iq_int32
   struct cint32_t
   {
      int32_t re;
      int32_t im;
   };

   /// @brief соответствует iq_format_t::iq_float32
   struct cfloat32_t
   {
      float re;
      float im;
   };
}//namespace UDP_IQ

#pragma pack(pop)


#endif // UDP_IQ_DEFINES_H
