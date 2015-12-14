/*
 * Chip version numbers taken from http://wiki/ChipVersionIDs.
 */

#ifndef CHIPVERSION_H
#define CHIPVERSION_H

typedef enum
{
    chip_bc01a = 0x01,
    chip_bc01b = 0x02,
    chip_bc02 = 0x03,
    chip_kato = 0x04,
    chip_kalimba = 0x05,
    chip_nicknack = 0x06,
    chip_paddywack = 0x07,
    chip_coyote = 0x08,
    chip_oddjob = 0x09,
    chip_pugwash = 0x0C,
    chip_stanley = 0x0D,
    chip_zebedee = 0x0E,
    chip_elvis = 0x10,
    chip_johnpeel = 0x11,
    chip_jumpinjack = 0x0F,
    chip_priscilla = 0x12,
    chip_herbie = 0x13,
    chip_fraggle = 0x14,
    chip_sugarlump = 0x15,
    chip_lisamarie = 0x16,
    chip_dash = 0x20,
    chip_jemima = 0x21,
    chip_cinderella = 0x22,
    chip_anastasia = 0x23
}
chipversion;

/* Chips that are known to suffer from B-35798: Xap2++ is missing break
 * exceptions. Used by BlueLab native debugging library to determine whether
 * they can use breakpoints. */
#define CHIP_MISSING_BREAK_EXCEPTIONS(version) \
    (   (version) == chip_stanley \
     || (version) == chip_elvis \
     || (version) == chip_johnpeel \
     || (version) == chip_priscilla \
     || (version) == chip_herbie \
     || (version) == chip_fraggle \
     || (version) == chip_sugarlump \
     || (version) == chip_jemima \
     || (version) == chip_dash )

#endif
