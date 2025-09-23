/*
 * my_nvic.h
 *
 *  Created on: Sep 22, 2025
 *      Author: ADMIN
 */

#ifndef MY_NVIC_H_
#define MY_NVIC_H_

#include <stdint.h>

#ifdef __cplusplus
  #define   __I     volatile             /*!< Defines 'read only' permissions */
#else
  #define   __I     volatile const       /*!< Defines 'read only' permissions */
#endif
#define     __O     volatile             /*!< Defines 'write only' permissions */
#define     __IO    volatile             /*!< Defines 'read / write' permissions */

/*
 * ==========================================================================
 * ================      Cortex-M4 NVIC Register Structures     ===============
 * ==========================================================================
 */

/**
  \brief  Structure type to access the Nested Vectored Interrupt Controller (NVIC).
 */
typedef struct
{
  __IO uint32_t ISER[8U];               /*!< Offset: 0x000 (R/W)  Interrupt Set Enable Register */
       uint32_t RESERVED0[24U];
  __IO uint32_t ICER[8U];               /*!< Offset: 0x080 (R/W)  Interrupt Clear Enable Register */
       uint32_t RESERVED1[24U];
  __IO uint32_t ISPR[8U];               /*!< Offset: 0x100 (R/W)  Interrupt Set Pending Register */
       uint32_t RESERVED2[24U];
  __IO uint32_t ICPR[8U];               /*!< Offset: 0x180 (R/W)  Interrupt Clear Pending Register */
       uint32_t RESERVED3[24U];
  __I  uint32_t IABR[8U];               /*!< Offset: 0x200 (R/ )  Interrupt Active bit Register */
       uint32_t RESERVED4[56U];
  __IO uint8_t  IP[240U];               /*!< Offset: 0x300 (R/W)  Interrupt Priority Register (8Bit wide) */
       uint32_t RESERVED5[644U];
  __O  uint32_t STIR;                    /*!< Offset: 0xE00 ( /W)  Software Trigger Interrupt Register */
}  NVIC_Type;


/* ==========================================================================
 * ================      Memory mapping and base pointers     ===============
 * ==========================================================================
 */

#define NVIC_BASE           (0xE000E100UL)

#define NVIC                ((NVIC_Type *) NVIC_BASE)



#endif /* MY_NVIC_H_ */
