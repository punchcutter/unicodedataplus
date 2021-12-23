/*
   Unicode character type helpers.

   Written by Marc-Andre Lemburg (mal@lemburg.com).
   Modified for Python 2.0 by Fredrik Lundh (fredrik@pythonware.com)

   Copyright (c) Corporation for National Research Initiatives.

*/

#include "Python.h"

#define ALPHA_MASK 0x01
#define DECIMAL_MASK 0x02
#define DIGIT_MASK 0x04
#define LOWER_MASK 0x08
#define LINEBREAK_MASK 0x10
#define SPACE_MASK 0x20
#define TITLE_MASK 0x40
#define UPPER_MASK 0x80
#define XID_START_MASK 0x100
#define XID_CONTINUE_MASK 0x200
#define PRINTABLE_MASK 0x400
#define NUMERIC_MASK 0x800
#define CASE_IGNORABLE_MASK 0x1000
#define CASED_MASK 0x2000
#define EXTENDED_CASE_MASK 0x4000

typedef struct {
    /*
       These are either deltas to the character or offsets in
       _PyUnicodePlus_ExtendedCase.
    */
    const int upper;
    const int lower;
    const int title;
    /* Note if more flag space is needed, decimal and digit could be unified. */
    const unsigned char decimal;
    const unsigned char digit;
    const unsigned short flags;
} _PyUnicodePlus_TypeRecord;

#include "unicodetype_db.h"

static const _PyUnicodePlus_TypeRecord *
gettyperecord(Py_UCS4 code)
{
    int index;

    if (code >= 0x110000)
        index = 0;
    else
    {
        index = index1[(code>>SHIFT)];
        index = index2[(index<<SHIFT)+(code&((1<<SHIFT)-1))];
    }

    return &_PyUnicodePlus_TypeRecords[index];
}

/* Returns the titlecase Unicode characters corresponding to ch or just
   ch if no titlecase mapping is known. */

Py_UCS4 _PyUnicodePlus_ToTitlecase(Py_UCS4 ch)
{
    const _PyUnicodePlus_TypeRecord *ctype = gettyperecord(ch);

    if (ctype->flags & EXTENDED_CASE_MASK)
        return _PyUnicodePlus_ExtendedCase[ctype->title & 0xFFFF];
    return ch + ctype->title;
}

/* Returns 1 for Unicode characters having the category 'Lt', 0
   otherwise. */

int _PyUnicodePlus_IsTitlecase(Py_UCS4 ch)
{
    const _PyUnicodePlus_TypeRecord *ctype = gettyperecord(ch);

    return (ctype->flags & TITLE_MASK) != 0;
}

/* Returns 1 for Unicode characters having the XID_Start property, 0
   otherwise. */

int _PyUnicodePlus_IsXidStart(Py_UCS4 ch)
{
    const _PyUnicodePlus_TypeRecord *ctype = gettyperecord(ch);

    return (ctype->flags & XID_START_MASK) != 0;
}

/* Returns 1 for Unicode characters having the XID_Continue property,
   0 otherwise. */

int _PyUnicodePlus_IsXidContinue(Py_UCS4 ch)
{
    const _PyUnicodePlus_TypeRecord *ctype = gettyperecord(ch);

    return (ctype->flags & XID_CONTINUE_MASK) != 0;
}

/* Returns the integer decimal (0-9) for Unicode characters having
   this property, -1 otherwise. */

int _PyUnicodePlus_ToDecimalDigit(Py_UCS4 ch)
{
    const _PyUnicodePlus_TypeRecord *ctype = gettyperecord(ch);

    return (ctype->flags & DECIMAL_MASK) ? ctype->decimal : -1;
}

int _PyUnicodePlus_IsDecimalDigit(Py_UCS4 ch)
{
    if (_PyUnicodePlus_ToDecimalDigit(ch) < 0)
        return 0;
    return 1;
}

/* Returns the integer digit (0-9) for Unicode characters having
   this property, -1 otherwise. */

int _PyUnicodePlus_ToDigit(Py_UCS4 ch)
{
    const _PyUnicodePlus_TypeRecord *ctype = gettyperecord(ch);

    return (ctype->flags & DIGIT_MASK) ? ctype->digit : -1;
}

int _PyUnicodePlus_IsDigit(Py_UCS4 ch)
{
    if (_PyUnicodePlus_ToDigit(ch) < 0)
        return 0;
    return 1;
}

/* Returns the numeric value as double for Unicode characters having
   this property, -1.0 otherwise. */

int _PyUnicodePlus_IsNumeric(Py_UCS4 ch)
{
    const _PyUnicodePlus_TypeRecord *ctype = gettyperecord(ch);

    return (ctype->flags & NUMERIC_MASK) != 0;
}

/* Returns 1 for Unicode characters to be hex-escaped when repr()ed,
   0 otherwise.
   All characters except those characters defined in the Unicode character
   database as following categories are considered printable.
      * Cc (Other, Control)
      * Cf (Other, Format)
      * Cs (Other, Surrogate)
      * Co (Other, Private Use)
      * Cn (Other, Not Assigned)
      * Zl Separator, Line ('\u2028', LINE SEPARATOR)
      * Zp Separator, Paragraph ('\u2029', PARAGRAPH SEPARATOR)
      * Zs (Separator, Space) other than ASCII space('\x20').
*/
int _PyUnicodePlus_IsPrintable(Py_UCS4 ch)
{
    const _PyUnicodePlus_TypeRecord *ctype = gettyperecord(ch);

    return (ctype->flags & PRINTABLE_MASK) != 0;
}

/* Returns 1 for Unicode characters having the category 'Ll', 0
   otherwise. */

int _PyUnicodePlus_IsLowercase(Py_UCS4 ch)
{
    const _PyUnicodePlus_TypeRecord *ctype = gettyperecord(ch);

    return (ctype->flags & LOWER_MASK) != 0;
}

/* Returns 1 for Unicode characters having the category 'Lu', 0
   otherwise. */

int _PyUnicodePlus_IsUppercase(Py_UCS4 ch)
{
    const _PyUnicodePlus_TypeRecord *ctype = gettyperecord(ch);

    return (ctype->flags & UPPER_MASK) != 0;
}

/* Returns the uppercase Unicode characters corresponding to ch or just
   ch if no uppercase mapping is known. */

Py_UCS4 _PyUnicodePlus_ToUppercase(Py_UCS4 ch)
{
    const _PyUnicodePlus_TypeRecord *ctype = gettyperecord(ch);

    if (ctype->flags & EXTENDED_CASE_MASK)
        return _PyUnicodePlus_ExtendedCase[ctype->upper & 0xFFFF];
    return ch + ctype->upper;
}

/* Returns the lowercase Unicode characters corresponding to ch or just
   ch if no lowercase mapping is known. */

Py_UCS4 _PyUnicodePlus_ToLowercase(Py_UCS4 ch)
{
    const _PyUnicodePlus_TypeRecord *ctype = gettyperecord(ch);

    if (ctype->flags & EXTENDED_CASE_MASK)
        return _PyUnicodePlus_ExtendedCase[ctype->lower & 0xFFFF];
    return ch + ctype->lower;
}

int _PyUnicodePlus_ToLowerFull(Py_UCS4 ch, Py_UCS4 *res)
{
    const _PyUnicodePlus_TypeRecord *ctype = gettyperecord(ch);

    if (ctype->flags & EXTENDED_CASE_MASK) {
        int index = ctype->lower & 0xFFFF;
        int n = ctype->lower >> 24;
        int i;
        for (i = 0; i < n; i++)
            res[i] = _PyUnicodePlus_ExtendedCase[index + i];
        return n;
    }
    res[0] = ch + ctype->lower;
    return 1;
}

int _PyUnicodePlus_ToTitleFull(Py_UCS4 ch, Py_UCS4 *res)
{
    const _PyUnicodePlus_TypeRecord *ctype = gettyperecord(ch);

    if (ctype->flags & EXTENDED_CASE_MASK) {
        int index = ctype->title & 0xFFFF;
        int n = ctype->title >> 24;
        int i;
        for (i = 0; i < n; i++)
            res[i] = _PyUnicodePlus_ExtendedCase[index + i];
        return n;
    }
    res[0] = ch + ctype->title;
    return 1;
}

int _PyUnicodePlus_ToUpperFull(Py_UCS4 ch, Py_UCS4 *res)
{
    const _PyUnicodePlus_TypeRecord *ctype = gettyperecord(ch);

    if (ctype->flags & EXTENDED_CASE_MASK) {
        int index = ctype->upper & 0xFFFF;
        int n = ctype->upper >> 24;
        int i;
        for (i = 0; i < n; i++)
            res[i] = _PyUnicodePlus_ExtendedCase[index + i];
        return n;
    }
    res[0] = ch + ctype->upper;
    return 1;
}

int _PyUnicodePlus_IsCased(Py_UCS4 ch)
{
    const _PyUnicodePlus_TypeRecord *ctype = gettyperecord(ch);

    return (ctype->flags & CASED_MASK) != 0;
}

int _PyUnicodePlus_IsCaseIgnorable(Py_UCS4 ch)
{
    const _PyUnicodePlus_TypeRecord *ctype = gettyperecord(ch);

    return (ctype->flags & CASE_IGNORABLE_MASK) != 0;
}

/* Returns 1 for Unicode characters having the category 'Ll', 'Lu', 'Lt',
   'Lo' or 'Lm',  0 otherwise. */

int _PyUnicodePlus_IsAlpha(Py_UCS4 ch)
{
    const _PyUnicodePlus_TypeRecord *ctype = gettyperecord(ch);

    return (ctype->flags & ALPHA_MASK) != 0;
}
