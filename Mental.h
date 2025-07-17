/*! \file mental.h
 * \brief Заголовочный файл с определениями стандартных типов и макросов для C и C++.
 *
 * Файл предоставляет определения стандартных типов и макросов для обеспечения переносимости
 * и удобства работы с целочисленными типами, указателями и размерами в программах на C и C++.
 * Поддерживает как C, так и C++ с использованием условной компиляции.
 */

#ifndef MENTAL_H
#define MENTAL_H

#ifdef __cplusplus
    #include <cstdint>
    #include <cstddef>
#else
    #include <stdint.h>
    #include <stddef.h>
    #include <stdbool.h>
    #include <stdlib.h>
    #include <stdio.h>
#endif

/*! \def mental.h versions.
 * \brief Макрос, определяющий версии mental.h
 */
#define MENTAL_H_VERSION_MAJOR 1
#define MENTAL_H_VERSION_MINOR 0
#define MENTAL_H_VERSION_PATCH 0

/*! \defgroup IntegerTypes Целочисленные типы
 * \brief Типы для работы с целыми числами фиксированной ширины.
 */

/*! \defgroup MaxIntegerTypes Максимальные целые типы
 * \brief Знаковые и беззнаковые целочисленные типы максимальной ширины.
 * \ingroup IntegerTypes
 * @{
 */

/*! \typedef MAX_INT
 * \brief Знаковый целочисленный тип максимальной ширины (`intmax_t`).
 */
typedef intmax_t        MAX_INT;

/*! \typedef MAX_UINT
 * \brief Беззнаковый целочисленный тип максимальной ширины (`uintmax_t`).
 */
typedef uintmax_t       MAX_UINT;
/*! @} */

/*! \defgroup StandardUnsignedIntegers Стандартные беззнаковые целые типы
 * \brief Беззнаковые целые типы фиксированной ширины.
 * \ingroup IntegerTypes
 * @{
 */

/*! \typedef u8
 * \brief 8-битный беззнаковый целый тип (`uint8_t`).
 */
typedef uint8_t         u8;

/*! \typedef u16
 * \brief 16-битный беззнаковый целый тип (`uint16_t`).
 */
typedef uint16_t        u16;

/*! \typedef u32
 * \brief 32-битный беззнаковый целый тип (`uint32_t`).
 */
typedef uint32_t        u32;

/*! \typedef u64
 * \brief 64-битный беззнаковый целый тип (`uint64_t`).
 */
typedef uint64_t        u64;
/*! @} */

/*! \defgroup StandardSignedIntegers Стандартные знаковые целые типы
 * \brief Знаковые целые типы фиксированной ширины.
 * \ingroup IntegerTypes
 * @{
 */

/*! \typedef i8
 * \brief 8-битный знаковый целый тип (`int8_t`).
 */
typedef int8_t      	i8;

/*! \typedef i16
 * \brief 16-битный знаковый целый тип (`int16_t`).
 */
typedef int16_t     	i16;

/*! \typedef i32
 * \brief 32-битный знаковый целый тип (`int32_t`).
 */
typedef int32_t     	i32;

/*! \typedef i64
 * \brief 64-битный знаковый целый тип (`int64_t`).
 */
typedef int64_t     	i64;
/*! @} */

/*! \defgroup FastIntegers Быстрые типы
 * \brief Типы, оптимизированные для скорости выполнения.
 * \ingroup IntegerTypes
 * @{
 */

/*! \typedef ufast8
 * \brief Быстрый 8-битный беззнаковый целый тип (`uint_fast8_t`).
 */
typedef uint_fast8_t    ufast8;

/*! \typedef ufast16
 * \brief Быстрый 16-битный беззнаковый целый тип (`uint_fast16_t`).
 */
typedef uint_fast16_t   ufast16;

/*! \typedef ufast32
 * \brief Быстрый 32-битный беззнаковый целый тип (`uint_fast32_t`).
 */
typedef uint_fast32_t   ufast32;

/*! \typedef ufast64
 * \brief Быстрый 64-битный беззнаковый целый тип (`uint_fast64_t`).
 */
typedef uint_fast64_t   ufast64;

/*! \typedef ifast8
 * \brief Быстрый 8-битный знаковый целый тип (`int_fast8_t`).
 */
typedef int_fast8_t     ifast8;

/*! \typedef ifast16
 * \brief Быстрый 16-битный знаковый целый тип (`int_fast16_t`).
 */
typedef int_fast16_t    ifast16;

/*! \typedef ifast32
 * \brief Быстрый 32-битный знаковый целый тип (`int_fast32_t`).
 */
typedef int_fast32_t    ifast32;

/*! \typedef ifast64
 * \brief Быстрый 64-битный знаковый целый тип (`int_fast64_t`).
 */
typedef int_fast64_t    ifast64;
/*! @} */

/*! \defgroup LeastIntegers Наименьшие типы
 * \brief Типы, гарантирующие минимальный размер памяти.
 * \ingroup IntegerTypes
 * @{
 */

/*! \typedef uleast8
 * \brief Наименьший 8-битный беззнаковый целый тип (`uint_least8_t`).
 */
typedef uint_least8_t   uleast8;

/*! \typedef uleast16
 * \brief Наименьший 16-битный беззнаковый целый тип (`uint_least16_t`).
 */
typedef uint_least16_t  uleast16;

/*! \typedef uleast32
 * \brief Наименьший 32-битный беззнаковый целый тип (`uint_least32_t`).
 */
typedef uint_least32_t  uleast32;

/*! \typedef uleast64
 * \brief Наименьший 64-битный беззнаковый целый тип (`uint_least64_t`).
 */
typedef uint_least64_t  uleast64;

/*! \typedef ileast8
 * \brief Наименьший 8-битный знаковый целый тип (`int_least8_t`).
 */
typedef int_least8_t    ileast8;

/*! \typedef ileast16
 * \brief Наименьший 16-битный знаковый целый тип (`int_least16_t`).
 */
typedef int_least16_t   ileast16;

/*! \typedef ileast32
 * \brief Наименьший 32-битный знаковый целый тип (`int_least32_t`).
 */
typedef int_least32_t   ileast32;

/*! \typedef ileast64
 * \brief Наименьший 64-битный знаковый целый тип (`int_least64_t`).
 */
typedef int_least64_t   ileast64;
/*! @} */

/*! \defgroup PointerTypes Типы для указателей
 * \brief Типы для представления указателей.
 * \ingroup IntegerTypes
 * @{
 */

/*! \typedef uptr
 * \brief Беззнаковый тип для представления указателей (`uintptr_t`).
 */
typedef uintptr_t       uptr;

/*! \typedef iptr
 * \brief Знаковый тип для представления указателей (`intptr_t`).
 */
typedef intptr_t        iptr;
/*! @} */

/*! \defgroup SizeTypes Типы для размеров
 * \brief Типы для представления размеров объектов и разницы указателей.
 * \ingroup IntegerTypes
 * @{
 */

/*! \typedef usize
 * \brief Беззнаковый тип для представления размеров объектов (аналог `sizeof`, `size_t`).
 */
typedef size_t          usize;

/*! \typedef isize
 * \brief Знаковый тип для представления разницы указателей (`ptrdiff_t`).
 */
typedef ptrdiff_t       isize;
/*! @} */

/*! \defgroup SpecialTypes Специальные типы
 * \brief Специальные типы для C и C++.
 * @{
 */

#ifdef __cplusplus
    /*! \typedef nullptr_t
     * \brief Тип для представления `nullptr` в C++ (`std::nullptr_t`).
     */
    using nullptr_t   = std::nullptr_t;
#endif

#if !defined(__cplusplus)
    /*! \def nullptr
     * \brief Макрос, определяющий `nullptr` как `NULL` в C.
     */
    #define nullptr NULL
#endif
/*! @} */

/*! \enum MentalResult
 * \brief Перечисление для представления кодов возврата функций.
 */
typedef enum MentalResult
{
    MENTAL_FATAL = -1,  /*!< Фатальная ошибка. */
    MENTAL_OK = 0,      /*!< Успешное выполнение. */
    MENTAL_ERROR = 1,   /*!< Ошибка выполнения. */
}
MentalResult;

#include "Mathematica.h"

#endif // MENTAL_H