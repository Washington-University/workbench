
0001        /*
0002         * Copyright 1994-2006 Sun Microsystems, Inc.  All Rights Reserved.
0003         * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
0004         *
0005         * This code is free software; you can redistribute it and/or modify it
0006         * under the terms of the GNU General Public License version 2 only, as
0007         * published by the Free Software Foundation.  Sun designates this
0008         * particular file as subject to the "Classpath" exception as provided
0009         * by Sun in the LICENSE file that accompanied this code.
0010         *
0011         * This code is distributed in the hope that it will be useful, but WITHOUT
0012         * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
0013         * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
0014         * version 2 for more details (a copy is included in the LICENSE file that
0015         * accompanied this code).
0016         *
0017         * You should have received a copy of the GNU General Public License version
0018         * 2 along with this work; if not, write to the Free Software Foundation,
0019         * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
0020         *
0021         * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa Clara,
0022         * CA 95054 USA or visit www.sun.com if you need additional information or
0023         * have any questions.
0024         */
0025
0026        package java.lang;
0027
0028        import java.io.ObjectStreamClass;
0029        import java.io.ObjectStreamField;
0030        import java.io.UnsupportedEncodingException;
0031        import java.nio.charset.Charset;
0032        import java.util.ArrayList;
0033        import java.util.Arrays;
0034        import java.util.Comparator;
0035        import java.util.Formatter;
0036        import java.util.Locale;
0037        import java.util.regex.Matcher;
0038        import java.util.regex.Pattern;
0039        import java.util.regex.PatternSyntaxException;
0040
0041        /**
0042         * The <code>String</code> class represents character strings. All
0043         * string literals in Java programs, such as <code>"abc"</code>, are
0044         * implemented as instances of this class.
0045         * <p>
0046         * Strings are constant; their values cannot be changed after they
0047         * are created. String buffers support mutable strings.
0048         * Because String objects are immutable they can be shared. For example:
0049         * <p><blockquote><pre>
0050         *     String str = "abc";
0051         * </pre></blockquote><p>
0052         * is equivalent to:
0053         * <p><blockquote><pre>
0054         *     char data[] = {'a', 'b', 'c'};
0055         *     String str = new String(data);
0056         * </pre></blockquote><p>
0057         * Here are some more examples of how strings can be used:
0058         * <p><blockquote><pre>
0059         *     System.out.println("abc");
0060         *     String cde = "cde";
0061         *     System.out.println("abc" + cde);
0062         *     String c = "abc".substring(2,3);
0063         *     String d = cde.substring(1, 2);
0064         * </pre></blockquote>
0065         * <p>
0066         * The class <code>String</code> includes methods for examining
0067         * individual characters of the sequence, for comparing strings, for
0068         * searching strings, for extracting substrings, and for creating a
0069         * copy of a string with all characters translated to uppercase or to
0070         * lowercase. Case mapping is based on the Unicode Standard version
0071         * specified by the {@link java.lang.Character Character} class.
0072         * <p>
0073         * The Java language provides special support for the string
0074         * concatenation operator (&nbsp;+&nbsp;), and for conversion of
0075         * other objects to strings. String concatenation is implemented
0076         * through the <code>StringBuilder</code>(or <code>StringBuffer</code>)
0077         * class and its <code>append</code> method.
0078         * String conversions are implemented through the method
0079         * <code>toString</code>, defined by <code>Object</code> and
0080         * inherited by all classes in Java. For additional information on
0081         * string concatenation and conversion, see Gosling, Joy, and Steele,
0082         * <i>The Java Language Specification</i>.
0083         *
0084         * <p> Unless otherwise noted, passing a <tt>null</tt> argument to a constructor
0085         * or method in this class will cause a {@link NullPointerException} to be
0086         * thrown.
0087         *
0088         * <p>A <code>String</code> represents a string in the UTF-16 format
0089         * in which <em>supplementary characters</em> are represented by <em>surrogate
0090         * pairs</em> (see the section <a href="Character.html#unicode">Unicode
0091         * Character Representations</a> in the <code>Character</code> class for
0092         * more information).
0093         * Index values refer to <code>char</code> code units, so a supplementary
0094         * character uses two positions in a <code>String</code>.
0095         * <p>The <code>String</code> class provides methods for dealing with
0096         * Unicode code points (i.e., characters), in addition to those for
0097         * dealing with Unicode code units (i.e., <code>char</code> values).
0098         *
0099         * @author  Lee Boynton
0100         * @author  Arthur van Hoff
0101         * @version 1.212, 05/05/07
0102         * @see     java.lang.Object#toString()
0103         * @see     java.lang.StringBuffer
0104         * @see     java.lang.StringBuilder
0105         * @see     java.nio.charset.Charset
0106         * @since   JDK1.0
0107         */
0108
0109        public final class String implements  java.io.Serializable,
0110                Comparable<String>, CharSequence {
0111            /** The value is used for character storage. */
0112            private final char value[];
0113
0114            /** The offset is the first index of the storage that is used. */
0115            private final int offset;
0116
0117            /** The count is the number of characters in the String. */
0118            private final int count;
0119
0120            /** Cache the hash code for the string */
0121            private int hash; // Default to 0
0122
0123            /** use serialVersionUID from JDK 1.0.2 for interoperability */
0124            private static final long serialVersionUID = -6849794470754667710L;
0125
0126            /**
0127             * Class String is special cased within the Serialization Stream Protocol.
0128             *
0129             * A String instance is written initially into an ObjectOutputStream in the
0130             * following format:
0131             * <pre>
0132             *      <code>TC_STRING</code> (utf String)
0133             * </pre>
0134             * The String is written by method <code>DataOutput.writeUTF</code>.
0135             * A new handle is generated to  refer to all future references to the
0136             * string instance within the stream.
0137             */
0138            private static final ObjectStreamField[] serialPersistentFields = new ObjectStreamField[0];
0139
0140            /**
0141             * Initializes a newly created {@code String} object so that it represents
0142             * an empty character sequence.  Note that use of this constructor is
0143             * unnecessary since Strings are immutable.
0144             */
0145            public String() {
0146                this .offset = 0;
0147                this .count = 0;
0148                this .value = new char[0];
0149            }
0150
0151            /**
0152             * Initializes a newly created {@code String} object so that it represents
0153             * the same sequence of characters as the argument; in other words, the
0154             * newly created string is a copy of the argument string. Unless an
0155             * explicit copy of {@code original} is needed, use of this constructor is
0156             * unnecessary since Strings are immutable.
0157             *
0158             * @param  original
0159             *         A {@code String}
0160             */
0161            public String(String original) {
0162                int size = original.count;
0163                char[] originalValue = original.value;
0164                char[] v;
0165                if (originalValue.length > size) {
0166                    // The array representing the String is bigger than the new
0167                    // String itself.  Perhaps this constructor is being called
0168                    // in order to trim the baggage, so make a copy of the array.
0169                    int off = original.offset;
0170                    v = Arrays.copyOfRange(originalValue, off, off + size);
0171                } else {
0172                    // The array representing the String is the same
0173                    // size as the String, so no point in making a copy.
0174                    v = originalValue;
0175                }
0176                this .offset = 0;
0177                this .count = size;
0178                this .value = v;
0179            }
0180
0181            /**
0182             * Allocates a new {@code String} so that it represents the sequence of
0183             * characters currently contained in the character array argument. The
0184             * contents of the character array are copied; subsequent modification of
0185             * the character array does not affect the newly created string.
0186             *
0187             * @param  value
0188             *         The initial value of the string
0189             */
0190            public String(char value[]) {
0191                int size = value.length;
0192                this .offset = 0;
0193                this .count = size;
0194                this .value = Arrays.copyOf(value, size);
0195            }
0196
0197            /**
0198             * Allocates a new {@code String} that contains characters from a subarray
0199             * of the character array argument. The {@code offset} argument is the
0200             * index of the first character of the subarray and the {@code count}
0201             * argument specifies the length of the subarray. The contents of the
0202             * subarray are copied; subsequent modification of the character array does
0203             * not affect the newly created string.
0204             *
0205             * @param  value
0206             *         Array that is the source of characters
0207             *
0208             * @param  offset
0209             *         The initial offset
0210             *
0211             * @param  count
0212             *         The length
0213             *
0214             * @throws  IndexOutOfBoundsException
0215             *          If the {@code offset} and {@code count} arguments index
0216             *          characters outside the bounds of the {@code value} array
0217             */
0218            public String(char value[], int offset, int count) {
0219                if (offset < 0) {
0220                    throw new StringIndexOutOfBoundsException(offset);
0221                }
0222                if (count < 0) {
0223                    throw new StringIndexOutOfBoundsException(count);
0224                }
0225                // Note: offset or count might be near -1>>>1.
0226                if (offset > value.length - count) {
0227                    throw new StringIndexOutOfBoundsException(offset + count);
0228                }
0229                this .offset = 0;
0230                this .count = count;
0231                this .value = Arrays.copyOfRange(value, offset, offset + count);
0232            }
0233
0234            /**
0235             * Allocates a new {@code String} that contains characters from a subarray
0236             * of the Unicode code point array argument. The {@code offset} argument
0237             * is the index of the first code point of the subarray and the
0238             * {@code count} argument specifies the length of the subarray. The
0239             * contents of the subarray are converted to {@code char}s; subsequent
0240             * modification of the {@code int} array does not affect the newly created
0241             * string.
0242             *
0243             * @param  codePoints
0244             *         Array that is the source of Unicode code points
0245             *
0246             * @param  offset
0247             *         The initial offset
0248             *
0249             * @param  count
0250             *         The length
0251             *
0252             * @throws  IllegalArgumentException
0253             *          If any invalid Unicode code point is found in {@code
0254             *          codePoints}
0255             *
0256             * @throws  IndexOutOfBoundsException
0257             *          If the {@code offset} and {@code count} arguments index
0258             *          characters outside the bounds of the {@code codePoints} array
0259             *
0260             * @since  1.5
0261             */
0262            public String(int[] codePoints, int offset, int count) {
0263                if (offset < 0) {
0264                    throw new StringIndexOutOfBoundsException(offset);
0265                }
0266                if (count < 0) {
0267                    throw new StringIndexOutOfBoundsException(count);
0268                }
0269                // Note: offset or count might be near -1>>>1.
0270                if (offset > codePoints.length - count) {
0271                    throw new StringIndexOutOfBoundsException(offset + count);
0272                }
0273
0274                int expansion = 0;
0275                int margin = 1;
0276                char[] v = new char[count + margin];
0277                int x = offset;
0278                int j = 0;
0279                for (int i = 0; i < count; i++) {
0280                    int c = codePoints[x++];
0281                    if (c < 0) {
0282                        throw new IllegalArgumentException();
0283                    }
0284                    if (margin <= 0 && (j + 1) >= v.length) {
0285                        if (expansion == 0) {
0286                            expansion = (((-margin + 1) * count) << 10) / i;
0287                            expansion >>= 10;
0288                            if (expansion <= 0) {
0289                                expansion = 1;
0290                            }
0291                        } else {
0292                            expansion *= 2;
0293                        }
0294                        int newLen = Math.min(v.length + expansion, count * 2);
0295                        margin = (newLen - v.length) - (count - i);
0296                        v = Arrays.copyOf(v, newLen);
0297                    }
0298                    if (c < Character.MIN_SUPPLEMENTARY_CODE_POINT) {
0299                        v[j++] = (char) c;
0300                    } else if (c <= Character.MAX_CODE_POINT) {
0301                        Character.toSurrogates(c, v, j);
0302                        j += 2;
0303                        margin--;
0304                    } else {
0305                        throw new IllegalArgumentException();
0306                    }
0307                }
0308                this .offset = 0;
0309                this .value = v;
0310                this .count = j;
0311            }
0312
0313            /**
0314             * Allocates a new {@code String} constructed from a subarray of an array
0315             * of 8-bit integer values.
0316             *
0317             * <p> The {@code offset} argument is the index of the first byte of the
0318             * subarray, and the {@code count} argument specifies the length of the
0319             * subarray.
0320             *
0321             * <p> Each {@code byte} in the subarray is converted to a {@code char} as
0322             * specified in the method above.
0323             *
0324             * @deprecated This method does not properly convert bytes into characters.
0325             * As of JDK&nbsp;1.1, the preferred way to do this is via the
0326             * {@code String} constructors that take a {@link
0327             * java.nio.charset.Charset}, charset name, or that use the platform's
0328             * default charset.
0329             *
0330             * @param  ascii
0331             *         The bytes to be converted to characters
0332             *
0333             * @param  hibyte
0334             *         The top 8 bits of each 16-bit Unicode code unit
0335             *
0336             * @param  offset
0337             *         The initial offset
0338             * @param  count
0339             *         The length
0340             *
0341             * @throws  IndexOutOfBoundsException
0342             *          If the {@code offset} or {@code count} argument is invalid
0343             *
0344             * @see  #String(byte[], int)
0345             * @see  #String(byte[], int, int, java.lang.String)
0346             * @see  #String(byte[], int, int, java.nio.charset.Charset)
0347             * @see  #String(byte[], int, int)
0348             * @see  #String(byte[], java.lang.String)
0349             * @see  #String(byte[], java.nio.charset.Charset)
0350             * @see  #String(byte[])
0351             */
0352            @Deprecated
0353            public String(byte ascii[], int hibyte, int offset, int count) {
0354                checkBounds(ascii, offset, count);
0355                char value[] = new char[count];
0356
0357                if (hibyte == 0) {
0358                    for (int i = count; i-- > 0;) {
0359                        value[i] = (char) (ascii[i + offset] & 0xff);
0360                    }
0361                } else {
0362                    hibyte <<= 8;
0363                    for (int i = count; i-- > 0;) {
0364                        value[i] = (char) (hibyte | (ascii[i + offset] & 0xff));
0365                    }
0366                }
0367                this .offset = 0;
0368                this .count = count;
0369                this .value = value;
0370            }
0371
0372            /**
0373             * Allocates a new {@code String} containing characters constructed from
0374             * an array of 8-bit integer values. Each character <i>c</i>in the
0375             * resulting string is constructed from the corresponding component
0376             * <i>b</i> in the byte array such that:
0377             *
0378             * <blockquote><pre>
0379             *     <b><i>c</i></b> == (char)(((hibyte &amp; 0xff) &lt;&lt; 8)
0380             *                         | (<b><i>b</i></b> &amp; 0xff))
0381             * </pre></blockquote>
0382             *
0383             * @deprecated  This method does not properly convert bytes into
0384             * characters.  As of JDK&nbsp;1.1, the preferred way to do this is via the
0385             * {@code String} constructors that take a {@link
0386             * java.nio.charset.Charset}, charset name, or that use the platform's
0387             * default charset.
0388             *
0389             * @param  ascii
0390             *         The bytes to be converted to characters
0391             *
0392             * @param  hibyte
0393             *         The top 8 bits of each 16-bit Unicode code unit
0394             *
0395             * @see  #String(byte[], int, int, java.lang.String)
0396             * @see  #String(byte[], int, int, java.nio.charset.Charset)
0397             * @see  #String(byte[], int, int)
0398             * @see  #String(byte[], java.lang.String)
0399             * @see  #String(byte[], java.nio.charset.Charset)
0400             * @see  #String(byte[])
0401             */
0402            @Deprecated
0403            public String(byte ascii[], int hibyte) {
0404                this (ascii, hibyte, 0, ascii.length);
0405            }
0406
0407            /* Common private utility method used to bounds check the byte array
0408             * and requested offset & length values used by the String(byte[],..)
0409             * constructors.
0410             */
0411            private static void checkBounds(byte[] bytes, int offset, int length) {
0412                if (length < 0)
0413                    throw new StringIndexOutOfBoundsException(length);
0414                if (offset < 0)
0415                    throw new StringIndexOutOfBoundsException(offset);
0416                if (offset > bytes.length - length)
0417                    throw new StringIndexOutOfBoundsException(offset + length);
0418            }
0419
0420            /**
0421             * Constructs a new {@code String} by decoding the specified subarray of
0422             * bytes using the specified charset.  The length of the new {@code String}
0423             * is a function of the charset, and hence may not be equal to the length
0424             * of the subarray.
0425             *
0426             * <p> The behavior of this constructor when the given bytes are not valid
0427             * in the given charset is unspecified.  The {@link
0428             * java.nio.charset.CharsetDecoder} class should be used when more control
0429             * over the decoding process is required.
0430             *
0431             * @param  bytes
0432             *         The bytes to be decoded into characters
0433             *
0434             * @param  offset
0435             *         The index of the first byte to decode
0436             *
0437             * @param  length
0438             *         The number of bytes to decode
0439
0440             * @param  charsetName
0441             *         The name of a supported {@linkplain java.nio.charset.Charset
0442             *         charset}
0443             *
0444             * @throws  UnsupportedEncodingException
0445             *          If the named charset is not supported
0446             *
0447             * @throws  IndexOutOfBoundsException
0448             *          If the {@code offset} and {@code length} arguments index
0449             *          characters outside the bounds of the {@code bytes} array
0450             *
0451             * @since  JDK1.1
0452             */
0453            public String(byte bytes[], int offset, int length,
0454                    String charsetName) throws UnsupportedEncodingException {
0455                if (charsetName == null)
0456                    throw new NullPointerException("charsetName");
0457                checkBounds(bytes, offset, length);
0458                char[] v = StringCoding.decode(charsetName, bytes, offset,
0459                        length);
0460                this .offset = 0;
0461                this .count = v.length;
0462                this .value = v;
0463            }
0464
0465            /**
0466             * Constructs a new {@code String} by decoding the specified subarray of
0467             * bytes using the specified {@linkplain java.nio.charset.Charset charset}.
0468             * The length of the new {@code String} is a function of the charset, and
0469             * hence may not be equal to the length of the subarray.
0470             *
0471             * <p> This method always replaces malformed-input and unmappable-character
0472             * sequences with this charset's default replacement string.  The {@link
0473             * java.nio.charset.CharsetDecoder} class should be used when more control
0474             * over the decoding process is required.
0475             *
0476             * @param  bytes
0477             *         The bytes to be decoded into characters
0478             *
0479             * @param  offset
0480             *         The index of the first byte to decode
0481             *
0482             * @param  length
0483             *         The number of bytes to decode
0484             *
0485             * @param  charset
0486             *         The {@linkplain java.nio.charset.Charset charset} to be used to
0487             *         decode the {@code bytes}
0488             *
0489             * @throws  IndexOutOfBoundsException
0490             *          If the {@code offset} and {@code length} arguments index
0491             *          characters outside the bounds of the {@code bytes} array
0492             *
0493             * @since  1.6
0494             */
0495            public String(byte bytes[], int offset, int length, Charset charset) {
0496                if (charset == null)
0497                    throw new NullPointerException("charset");
0498                checkBounds(bytes, offset, length);
0499                char[] v = StringCoding.decode(charset, bytes, offset, length);
0500                this .offset = 0;
0501                this .count = v.length;
0502                this .value = v;
0503            }
0504
0505            /**
0506             * Constructs a new {@code String} by decoding the specified array of bytes
0507             * using the specified {@linkplain java.nio.charset.Charset charset}.  The
0508             * length of the new {@code String} is a function of the charset, and hence
0509             * may not be equal to the length of the byte array.
0510             *
0511             * <p> The behavior of this constructor when the given bytes are not valid
0512             * in the given charset is unspecified.  The {@link
0513             * java.nio.charset.CharsetDecoder} class should be used when more control
0514             * over the decoding process is required.
0515             *
0516             * @param  bytes
0517             *         The bytes to be decoded into characters
0518             *
0519             * @param  charsetName
0520             *         The name of a supported {@linkplain java.nio.charset.Charset
0521             *         charset}
0522             *
0523             * @throws  UnsupportedEncodingException
0524             *          If the named charset is not supported
0525             *
0526             * @since  JDK1.1
0527             */
0528            public String(byte bytes[], String charsetName)
0529                    throws UnsupportedEncodingException {
0530                this (bytes, 0, bytes.length, charsetName);
0531            }
0532
0533            /**
0534             * Constructs a new {@code String} by decoding the specified array of
0535             * bytes using the specified {@linkplain java.nio.charset.Charset charset}.
0536             * The length of the new {@code String} is a function of the charset, and
0537             * hence may not be equal to the length of the byte array.
0538             *
0539             * <p> This method always replaces malformed-input and unmappable-character
0540             * sequences with this charset's default replacement string.  The {@link
0541             * java.nio.charset.CharsetDecoder} class should be used when more control
0542             * over the decoding process is required.
0543             *
0544             * @param  bytes
0545             *         The bytes to be decoded into characters
0546             *
0547             * @param  charset
0548             *         The {@linkplain java.nio.charset.Charset charset} to be used to
0549             *         decode the {@code bytes}
0550             *
0551             * @since  1.6
0552             */
0553            public String(byte bytes[], Charset charset) {
0554                this (bytes, 0, bytes.length, charset);
0555            }
0556
0557            /**
0558             * Constructs a new {@code String} by decoding the specified subarray of
0559             * bytes using the platform's default charset.  The length of the new
0560             * {@code String} is a function of the charset, and hence may not be equal
0561             * to the length of the subarray.
0562             *
0563             * <p> The behavior of this constructor when the given bytes are not valid
0564             * in the default charset is unspecified.  The {@link
0565             * java.nio.charset.CharsetDecoder} class should be used when more control
0566             * over the decoding process is required.
0567             *
0568             * @param  bytes
0569             *         The bytes to be decoded into characters
0570             *
0571             * @param  offset
0572             *         The index of the first byte to decode
0573             *
0574             * @param  length
0575             *         The number of bytes to decode
0576             *
0577             * @throws  IndexOutOfBoundsException
0578             *          If the {@code offset} and the {@code length} arguments index
0579             *          characters outside the bounds of the {@code bytes} array
0580             *
0581             * @since  JDK1.1
0582             */
0583            public String(byte bytes[], int offset, int length) {
0584                checkBounds(bytes, offset, length);
0585                char[] v = StringCoding.decode(bytes, offset, length);
0586                this .offset = 0;
0587                this .count = v.length;
0588                this .value = v;
0589            }
0590
0591            /**
0592             * Constructs a new {@code String} by decoding the specified array of bytes
0593             * using the platform's default charset.  The length of the new {@code
0594             * String} is a function of the charset, and hence may not be equal to the
0595             * length of the byte array.
0596             *
0597             * <p> The behavior of this constructor when the given bytes are not valid
0598             * in the default charset is unspecified.  The {@link
0599             * java.nio.charset.CharsetDecoder} class should be used when more control
0600             * over the decoding process is required.
0601             *
0602             * @param  bytes
0603             *         The bytes to be decoded into characters
0604             *
0605             * @since  JDK1.1
0606             */
0607            public String(byte bytes[]) {
0608                this (bytes, 0, bytes.length);
0609            }
0610
0611            /**
0612             * Allocates a new string that contains the sequence of characters
0613             * currently contained in the string buffer argument. The contents of the
0614             * string buffer are copied; subsequent modification of the string buffer
0615             * does not affect the newly created string.
0616             *
0617             * @param  buffer
0618             *         A {@code StringBuffer}
0619             */
0620            public String(StringBuffer buffer) {
0621                String result = buffer.toString();
0622                this .value = result.value;
0623                this .count = result.count;
0624                this .offset = result.offset;
0625            }
0626
0627            /**
0628             * Allocates a new string that contains the sequence of characters
0629             * currently contained in the string builder argument. The contents of the
0630             * string builder are copied; subsequent modification of the string builder
0631             * does not affect the newly created string.
0632             *
0633             * <p> This constructor is provided to ease migration to {@code
0634             * StringBuilder}. Obtaining a string from a string builder via the {@code
0635             * toString} method is likely to run faster and is generally preferred.
0636             *
0637             * @param   builder
0638             *          A {@code StringBuilder}
0639             *
0640             * @since  1.5
0641             */
0642            public String(StringBuilder builder) {
0643                String result = builder.toString();
0644                this .value = result.value;
0645                this .count = result.count;
0646                this .offset = result.offset;
0647            }
0648
0649            // Package private constructor which shares value array for speed.
0650            String(int offset, int count, char value[]) {
0651                this .value = value;
0652                this .offset = offset;
0653                this .count = count;
0654            }
0655
0656            /**
0657             * Returns the length of this string.
0658             * The length is equal to the number of <a href="Character.html#unicode">Unicode
0659             * code units</a> in the string.
0660             *
0661             * @return  the length of the sequence of characters represented by this
0662             *          object.
0663             */
0664            public int length() {
0665                return count;
0666            }
0667
0668            /**
0669             * Returns <tt>true</tt> if, and only if, {@link #length()} is <tt>0</tt>.
0670             *
0671             * @return <tt>true</tt> if {@link #length()} is <tt>0</tt>, otherwise
0672             * <tt>false</tt>
0673             *
0674             * @since 1.6
0675             */
0676            public boolean isEmpty() {
0677                return count == 0;
0678            }
0679
0680            /**
0681             * Returns the <code>char</code> value at the
0682             * specified index. An index ranges from <code>0</code> to
0683             * <code>length() - 1</code>. The first <code>char</code> value of the sequence
0684             * is at index <code>0</code>, the next at index <code>1</code>,
0685             * and so on, as for array indexing.
0686             *
0687             * <p>If the <code>char</code> value specified by the index is a
0688             * <a href="Character.html#unicode">surrogate</a>, the surrogate
0689             * value is returned.
0690             *
0691             * @param      index   the index of the <code>char</code> value.
0692             * @return     the <code>char</code> value at the specified index of this string.
0693             *             The first <code>char</code> value is at index <code>0</code>.
0694             * @exception  IndexOutOfBoundsException  if the <code>index</code>
0695             *             argument is negative or not less than the length of this
0696             *             string.
0697             */
0698            public char charAt(int index) {
0699                if ((index < 0) || (index >= count)) {
0700                    throw new StringIndexOutOfBoundsException(index);
0701                }
0702                return value[index + offset];
0703            }
0704
0705            /**
0706             * Returns the character (Unicode code point) at the specified
0707             * index. The index refers to <code>char</code> values
0708             * (Unicode code units) and ranges from <code>0</code> to
0709             * {@link #length()}<code> - 1</code>.
0710             *
0711             * <p> If the <code>char</code> value specified at the given index
0712             * is in the high-surrogate range, the following index is less
0713             * than the length of this <code>String</code>, and the
0714             * <code>char</code> value at the following index is in the
0715             * low-surrogate range, then the supplementary code point
0716             * corresponding to this surrogate pair is returned. Otherwise,
0717             * the <code>char</code> value at the given index is returned.
0718             *
0719             * @param      index the index to the <code>char</code> values
0720             * @return     the code point value of the character at the
0721             *             <code>index</code>
0722             * @exception  IndexOutOfBoundsException  if the <code>index</code>
0723             *             argument is negative or not less than the length of this
0724             *             string.
0725             * @since      1.5
0726             */
0727            public int codePointAt(int index) {
0728                if ((index < 0) || (index >= count)) {
0729                    throw new StringIndexOutOfBoundsException(index);
0730                }
0731                return Character.codePointAtImpl(value, offset + index, offset
0732                        + count);
0733            }
0734
0735            /**
0736             * Returns the character (Unicode code point) before the specified
0737             * index. The index refers to <code>char</code> values
0738             * (Unicode code units) and ranges from <code>1</code> to {@link
0739             * CharSequence#length() length}.
0740             *
0741             * <p> If the <code>char</code> value at <code>(index - 1)</code>
0742             * is in the low-surrogate range, <code>(index - 2)</code> is not
0743             * negative, and the <code>char</code> value at <code>(index -
0744             * 2)</code> is in the high-surrogate range, then the
0745             * supplementary code point value of the surrogate pair is
0746             * returned. If the <code>char</code> value at <code>index -
0747             * 1</code> is an unpaired low-surrogate or a high-surrogate, the
0748             * surrogate value is returned.
0749             *
0750             * @param     index the index following the code point that should be returned
0751             * @return    the Unicode code point value before the given index.
0752             * @exception IndexOutOfBoundsException if the <code>index</code>
0753             *            argument is less than 1 or greater than the length
0754             *            of this string.
0755             * @since     1.5
0756             */
0757            public int codePointBefore(int index) {
0758                int i = index - 1;
0759                if ((i < 0) || (i >= count)) {
0760                    throw new StringIndexOutOfBoundsException(index);
0761                }
0762                return Character.codePointBeforeImpl(value, offset + index,
0763                        offset);
0764            }
0765
0766            /**
0767             * Returns the number of Unicode code points in the specified text
0768             * range of this <code>String</code>. The text range begins at the
0769             * specified <code>beginIndex</code> and extends to the
0770             * <code>char</code> at index <code>endIndex - 1</code>. Thus the
0771             * length (in <code>char</code>s) of the text range is
0772             * <code>endIndex-beginIndex</code>. Unpaired surrogates within
0773             * the text range count as one code point each.
0774             *
0775             * @param beginIndex the index to the first <code>char</code> of
0776             * the text range.
0777             * @param endIndex the index after the last <code>char</code> of
0778             * the text range.
0779             * @return the number of Unicode code points in the specified text
0780             * range
0781             * @exception IndexOutOfBoundsException if the
0782             * <code>beginIndex</code> is negative, or <code>endIndex</code>
0783             * is larger than the length of this <code>String</code>, or
0784             * <code>beginIndex</code> is larger than <code>endIndex</code>.
0785             * @since  1.5
0786             */
0787            public int codePointCount(int beginIndex, int endIndex) {
0788                if (beginIndex < 0 || endIndex > count || beginIndex > endIndex) {
0789                    throw new IndexOutOfBoundsException();
0790                }
0791                return Character.codePointCountImpl(value, offset + beginIndex,
0792                        endIndex - beginIndex);
0793            }
0794
0795            /**
0796             * Returns the index within this <code>String</code> that is
0797             * offset from the given <code>index</code> by
0798             * <code>codePointOffset</code> code points. Unpaired surrogates
0799             * within the text range given by <code>index</code> and
0800             * <code>codePointOffset</code> count as one code point each.
0801             *
0802             * @param index the index to be offset
0803             * @param codePointOffset the offset in code points
0804             * @return the index within this <code>String</code>
0805             * @exception IndexOutOfBoundsException if <code>index</code>
0806             *   is negative or larger then the length of this
0807             *   <code>String</code>, or if <code>codePointOffset</code> is positive
0808             *   and the substring starting with <code>index</code> has fewer
0809             *   than <code>codePointOffset</code> code points,
0810             *   or if <code>codePointOffset</code> is negative and the substring
0811             *   before <code>index</code> has fewer than the absolute value
0812             *   of <code>codePointOffset</code> code points.
0813             * @since 1.5
0814             */
0815            public int offsetByCodePoints(int index, int codePointOffset) {
0816                if (index < 0 || index > count) {
0817                    throw new IndexOutOfBoundsException();
0818                }
0819                return Character.offsetByCodePointsImpl(value, offset, count,
0820                        offset + index, codePointOffset)
0821                        - offset;
0822            }
0823
0824            /**
0825             * Copy characters from this string into dst starting at dstBegin.
0826             * This method doesn't perform any range checking.
0827             */
0828            void getChars(char dst[], int dstBegin) {
0829                System.arraycopy(value, offset, dst, dstBegin, count);
0830            }
0831
0832            /**
0833             * Copies characters from this string into the destination character
0834             * array.
0835             * <p>
0836             * The first character to be copied is at index <code>srcBegin</code>;
0837             * the last character to be copied is at index <code>srcEnd-1</code>
0838             * (thus the total number of characters to be copied is
0839             * <code>srcEnd-srcBegin</code>). The characters are copied into the
0840             * subarray of <code>dst</code> starting at index <code>dstBegin</code>
0841             * and ending at index:
0842             * <p><blockquote><pre>
0843             *     dstbegin + (srcEnd-srcBegin) - 1
0844             * </pre></blockquote>
0845             *
0846             * @param      srcBegin   index of the first character in the string
0847             *                        to copy.
0848             * @param      srcEnd     index after the last character in the string
0849             *                        to copy.
0850             * @param      dst        the destination array.
0851             * @param      dstBegin   the start offset in the destination array.
0852             * @exception IndexOutOfBoundsException If any of the following
0853             *            is true:
0854             *            <ul><li><code>srcBegin</code> is negative.
0855             *            <li><code>srcBegin</code> is greater than <code>srcEnd</code>
0856             *            <li><code>srcEnd</code> is greater than the length of this
0857             *                string
0858             *            <li><code>dstBegin</code> is negative
0859             *            <li><code>dstBegin+(srcEnd-srcBegin)</code> is larger than
0860             *                <code>dst.length</code></ul>
0861             */
0862            public void getChars(int srcBegin, int srcEnd, char dst[],
0863                    int dstBegin) {
0864                if (srcBegin < 0) {
0865                    throw new StringIndexOutOfBoundsException(srcBegin);
0866                }
0867                if (srcEnd > count) {
0868                    throw new StringIndexOutOfBoundsException(srcEnd);
0869                }
0870                if (srcBegin > srcEnd) {
0871                    throw new StringIndexOutOfBoundsException(srcEnd - srcBegin);
0872                }
0873                System.arraycopy(value, offset + srcBegin, dst, dstBegin,
0874                        srcEnd - srcBegin);
0875            }
0876
0877            /**
0878             * Copies characters from this string into the destination byte array. Each
0879             * byte receives the 8 low-order bits of the corresponding character. The
0880             * eight high-order bits of each character are not copied and do not
0881             * participate in the transfer in any way.
0882             *
0883             * <p> The first character to be copied is at index {@code srcBegin}; the
0884             * last character to be copied is at index {@code srcEnd-1}.  The total
0885             * number of characters to be copied is {@code srcEnd-srcBegin}. The
0886             * characters, converted to bytes, are copied into the subarray of {@code
0887             * dst} starting at index {@code dstBegin} and ending at index:
0888             *
0889             * <blockquote><pre>
0890             *     dstbegin + (srcEnd-srcBegin) - 1
0891             * </pre></blockquote>
0892             *
0893             * @deprecated  This method does not properly convert characters into
0894             * bytes.  As of JDK&nbsp;1.1, the preferred way to do this is via the
0895             * {@link #getBytes()} method, which uses the platform's default charset.
0896             *
0897             * @param  srcBegin
0898             *         Index of the first character in the string to copy
0899             *
0900             * @param  srcEnd
0901             *         Index after the last character in the string to copy
0902             *
0903             * @param  dst
0904             *         The destination array
0905             *
0906             * @param  dstBegin
0907             *         The start offset in the destination array
0908             *
0909             * @throws  IndexOutOfBoundsException
0910             *          If any of the following is true:
0911             *          <ul>
0912             *            <li> {@code srcBegin} is negative
0913             *            <li> {@code srcBegin} is greater than {@code srcEnd}
0914             *            <li> {@code srcEnd} is greater than the length of this String
0915             *            <li> {@code dstBegin} is negative
0916             *            <li> {@code dstBegin+(srcEnd-srcBegin)} is larger than {@code
0917             *                 dst.length}
0918             *          </ul>
0919             */
0920            @Deprecated
0921            public void getBytes(int srcBegin, int srcEnd, byte dst[],
0922                    int dstBegin) {
0923                if (srcBegin < 0) {
0924                    throw new StringIndexOutOfBoundsException(srcBegin);
0925                }
0926                if (srcEnd > count) {
0927                    throw new StringIndexOutOfBoundsException(srcEnd);
0928                }
0929                if (srcBegin > srcEnd) {
0930                    throw new StringIndexOutOfBoundsException(srcEnd - srcBegin);
0931                }
0932                int j = dstBegin;
0933                int n = offset + srcEnd;
0934                int i = offset + srcBegin;
0935                char[] val = value; /* avoid getfield opcode */
0936
0937                while (i < n) {
0938                    dst[j++] = (byte) val[i++];
0939                }
0940            }
0941
0942            /**
0943             * Encodes this {@code String} into a sequence of bytes using the named
0944             * charset, storing the result into a new byte array.
0945             *
0946             * <p> The behavior of this method when this string cannot be encoded in
0947             * the given charset is unspecified.  The {@link
0948             * java.nio.charset.CharsetEncoder} class should be used when more control
0949             * over the encoding process is required.
0950             *
0951             * @param  charsetName
0952             *         The name of a supported {@linkplain java.nio.charset.Charset
0953             *         charset}
0954             *
0955             * @return  The resultant byte array
0956             *
0957             * @throws  UnsupportedEncodingException
0958             *          If the named charset is not supported
0959             *
0960             * @since  JDK1.1
0961             */
0962            public byte[] getBytes(String charsetName)
0963                    throws UnsupportedEncodingException {
0964                if (charsetName == null)
0965                    throw new NullPointerException();
0966                return StringCoding.encode(charsetName, value, offset, count);
0967            }
0968
0969            /**
0970             * Encodes this {@code String} into a sequence of bytes using the given
0971             * {@linkplain java.nio.charset.Charset charset}, storing the result into a
0972             * new byte array.
0973             *
0974             * <p> This method always replaces malformed-input and unmappable-character
0975             * sequences with this charset's default replacement byte array.  The
0976             * {@link java.nio.charset.CharsetEncoder} class should be used when more
0977             * control over the encoding process is required.
0978             *
0979             * @param  charset
0980             *         The {@linkplain java.nio.charset.Charset} to be used to encode
0981             *         the {@code String}
0982             *
0983             * @return  The resultant byte array
0984             *
0985             * @since  1.6
0986             */
0987            public byte[] getBytes(Charset charset) {
0988                if (charset == null)
0989                    throw new NullPointerException();
0990                return StringCoding.encode(charset, value, offset, count);
0991            }
0992
0993            /**
0994             * Encodes this {@code String} into a sequence of bytes using the
0995             * platform's default charset, storing the result into a new byte array.
0996             *
0997             * <p> The behavior of this method when this string cannot be encoded in
0998             * the default charset is unspecified.  The {@link
0999             * java.nio.charset.CharsetEncoder} class should be used when more control
1000             * over the encoding process is required.
1001             *
1002             * @return  The resultant byte array
1003             *
1004             * @since      JDK1.1
1005             */
1006            public byte[] getBytes() {
1007                return StringCoding.encode(value, offset, count);
1008            }
1009
1010            /**
1011             * Compares this string to the specified object.  The result is {@code
1012             * true} if and only if the argument is not {@code null} and is a {@code
1013             * String} object that represents the same sequence of characters as this
1014             * object.
1015             *
1016             * @param  anObject
1017             *         The object to compare this {@code String} against
1018             *
1019             * @return  {@code true} if the given object represents a {@code String}
1020             *          equivalent to this string, {@code false} otherwise
1021             *
1022             * @see  #compareTo(String)
1023             * @see  #equalsIgnoreCase(String)
1024             */
1025            public boolean equals(Object anObject) {
1026                if (this  == anObject) {
1027                    return true;
1028                }
1029                if (anObject instanceof  String) {
1030                    String anotherString = (String) anObject;
1031                    int n = count;
1032                    if (n == anotherString.count) {
1033                        char v1[] = value;
1034                        char v2[] = anotherString.value;
1035                        int i = offset;
1036                        int j = anotherString.offset;
1037                        while (n-- != 0) {
1038                            if (v1[i++] != v2[j++])
1039                                return false;
1040                        }
1041                        return true;
1042                    }
1043                }
1044                return false;
1045            }
1046
1047            /**
1048             * Compares this string to the specified {@code StringBuffer}.  The result
1049             * is {@code true} if and only if this {@code String} represents the same
1050             * sequence of characters as the specified {@code StringBuffer}.
1051             *
1052             * @param  sb
1053             *         The {@code StringBuffer} to compare this {@code String} against
1054             *
1055             * @return  {@code true} if this {@code String} represents the same
1056             *          sequence of characters as the specified {@code StringBuffer},
1057             *          {@code false} otherwise
1058             *
1059             * @since  1.4
1060             */
1061            public boolean contentEquals(StringBuffer sb) {
1062                synchronized (sb) {
1063                    return contentEquals((CharSequence) sb);
1064                }
1065            }
1066
1067            /**
1068             * Compares this string to the specified {@code CharSequence}.  The result
1069             * is {@code true} if and only if this {@code String} represents the same
1070             * sequence of char values as the specified sequence.
1071             *
1072             * @param  cs
1073             *         The sequence to compare this {@code String} against
1074             *
1075             * @return  {@code true} if this {@code String} represents the same
1076             *          sequence of char values as the specified sequence, {@code
1077             *          false} otherwise
1078             *
1079             * @since  1.5
1080             */
1081            public boolean contentEquals(CharSequence cs) {
1082                if (count != cs.length())
1083                    return false;
1084                // Argument is a StringBuffer, StringBuilder
1085                if (cs instanceof  AbstractStringBuilder) {
1086                    char v1[] = value;
1087                    char v2[] = ((AbstractStringBuilder) cs).getValue();
1088                    int i = offset;
1089                    int j = 0;
1090                    int n = count;
1091                    while (n-- != 0) {
1092                        if (v1[i++] != v2[j++])
1093                            return false;
1094                    }
1095                    return true;
1096                }
1097                // Argument is a String
1098                if (cs.equals(this ))
1099                    return true;
1100                // Argument is a generic CharSequence
1101                char v1[] = value;
1102                int i = offset;
1103                int j = 0;
1104                int n = count;
1105                while (n-- != 0) {
1106                    if (v1[i++] != cs.charAt(j++))
1107                        return false;
1108                }
1109                return true;
1110            }
1111
1112            /**
1113             * Compares this {@code String} to another {@code String}, ignoring case
1114             * considerations.  Two strings are considered equal ignoring case if they
1115             * are of the same length and corresponding characters in the two strings
1116             * are equal ignoring case.
1117             *
1118             * <p> Two characters {@code c1} and {@code c2} are considered the same
1119             * ignoring case if at least one of the following is true:
1120             * <ul>
1121             *   <li> The two characters are the same (as compared by the
1122             *        {@code ==} operator)
1123             *   <li> Applying the method {@link
1124             *        java.lang.Character#toUpperCase(char)} to each character
1125             *        produces the same result
1126             *   <li> Applying the method {@link
1127             *        java.lang.Character#toLowerCase(char)} to each character
1128             *        produces the same result
1129             * </ul>
1130             *
1131             * @param  anotherString
1132             *         The {@code String} to compare this {@code String} against
1133             *
1134             * @return  {@code true} if the argument is not {@code null} and it
1135             *          represents an equivalent {@code String} ignoring case; {@code
1136             *          false} otherwise
1137             *
1138             * @see  #equals(Object)
1139             */
1140            public boolean equalsIgnoreCase(String anotherString) {
1141                return (this  == anotherString) ? true : (anotherString != null)
1142                        && (anotherString.count == count)
1143                        && regionMatches(true, 0, anotherString, 0, count);
1144            }
1145
1146            /**
1147             * Compares two strings lexicographically.
1148             * The comparison is based on the Unicode value of each character in
1149             * the strings. The character sequence represented by this
1150             * <code>String</code> object is compared lexicographically to the
1151             * character sequence represented by the argument string. The result is
1152             * a negative integer if this <code>String</code> object
1153             * lexicographically precedes the argument string. The result is a
1154             * positive integer if this <code>String</code> object lexicographically
1155             * follows the argument string. The result is zero if the strings
1156             * are equal; <code>compareTo</code> returns <code>0</code> exactly when
1157             * the {@link #equals(Object)} method would return <code>true</code>.
1158             * <p>
1159             * This is the definition of lexicographic ordering. If two strings are
1160             * different, then either they have different characters at some index
1161             * that is a valid index for both strings, or their lengths are different,
1162             * or both. If they have different characters at one or more index
1163             * positions, let <i>k</i> be the smallest such index; then the string
1164             * whose character at position <i>k</i> has the smaller value, as
1165             * determined by using the &lt; operator, lexicographically precedes the
1166             * other string. In this case, <code>compareTo</code> returns the
1167             * difference of the two character values at position <code>k</code> in
1168             * the two string -- that is, the value:
1169             * <blockquote><pre>
1170             * this.charAt(k)-anotherString.charAt(k)
1171             * </pre></blockquote>
1172             * If there is no index position at which they differ, then the shorter
1173             * string lexicographically precedes the longer string. In this case,
1174             * <code>compareTo</code> returns the difference of the lengths of the
1175             * strings -- that is, the value:
1176             * <blockquote><pre>
1177             * this.length()-anotherString.length()
1178             * </pre></blockquote>
1179             *
1180             * @param   anotherString   the <code>String</code> to be compared.
1181             * @return  the value <code>0</code> if the argument string is equal to
1182             *          this string; a value less than <code>0</code> if this string
1183             *          is lexicographically less than the string argument; and a
1184             *          value greater than <code>0</code> if this string is
1185             *          lexicographically greater than the string argument.
1186             */
1187            public int compareTo(String anotherString) {
1188                int len1 = count;
1189                int len2 = anotherString.count;
1190                int n = Math.min(len1, len2);
1191                char v1[] = value;
1192                char v2[] = anotherString.value;
1193                int i = offset;
1194                int j = anotherString.offset;
1195
1196                if (i == j) {
1197                    int k = i;
1198                    int lim = n + i;
1199                    while (k < lim) {
1200                        char c1 = v1[k];
1201                        char c2 = v2[k];
1202                        if (c1 != c2) {
1203                            return c1 - c2;
1204                        }
1205                        k++;
1206                    }
1207                } else {
1208                    while (n-- != 0) {
1209                        char c1 = v1[i++];
1210                        char c2 = v2[j++];
1211                        if (c1 != c2) {
1212                            return c1 - c2;
1213                        }
1214                    }
1215                }
1216                return len1 - len2;
1217            }
1218
1219            /**
1220             * A Comparator that orders <code>String</code> objects as by
1221             * <code>compareToIgnoreCase</code>. This comparator is serializable.
1222             * <p>
1223             * Note that this Comparator does <em>not</em> take locale into account,
1224             * and will result in an unsatisfactory ordering for certain locales.
1225             * The java.text package provides <em>Collators</em> to allow
1226             * locale-sensitive ordering.
1227             *
1228             * @see     java.text.Collator#compare(String, String)
1229             * @since   1.2
1230             */
1231            public static final Comparator<String> CASE_INSENSITIVE_ORDER = new CaseInsensitiveComparator();
1232
1233            private static class CaseInsensitiveComparator implements 
1234                    Comparator<String>, java.io.Serializable {
1235                // use serialVersionUID from JDK 1.2.2 for interoperability
1236                private static final long serialVersionUID = 8575799808933029326L;
1237
1238                public int compare(String s1, String s2) {
1239                    int n1 = s1.length();
1240                    int n2 = s2.length();
1241                    int min = Math.min(n1, n2);
1242                    for (int i = 0; i < min; i++) {
1243                        char c1 = s1.charAt(i);
1244                        char c2 = s2.charAt(i);
1245                        if (c1 != c2) {
1246                            c1 = Character.toUpperCase(c1);
1247                            c2 = Character.toUpperCase(c2);
1248                            if (c1 != c2) {
1249                                c1 = Character.toLowerCase(c1);
1250                                c2 = Character.toLowerCase(c2);
1251                                if (c1 != c2) {
1252                                    // No overflow because of numeric promotion
1253                                    return c1 - c2;
1254                                }
1255                            }
1256                        }
1257                    }
1258                    return n1 - n2;
1259                }
1260            }
1261
1262            /**
1263             * Compares two strings lexicographically, ignoring case
1264             * differences. This method returns an integer whose sign is that of
1265             * calling <code>compareTo</code> with normalized versions of the strings
1266             * where case differences have been eliminated by calling
1267             * <code>Character.toLowerCase(Character.toUpperCase(character))</code> on
1268             * each character.
1269             * <p>
1270             * Note that this method does <em>not</em> take locale into account,
1271             * and will result in an unsatisfactory ordering for certain locales.
1272             * The java.text package provides <em>collators</em> to allow
1273             * locale-sensitive ordering.
1274             *
1275             * @param   str   the <code>String</code> to be compared.
1276             * @return  a negative integer, zero, or a positive integer as the
1277             *		specified String is greater than, equal to, or less
1278             *		than this String, ignoring case considerations.
1279             * @see     java.text.Collator#compare(String, String)
1280             * @since   1.2
1281             */
1282            public int compareToIgnoreCase(String str) {
1283                return CASE_INSENSITIVE_ORDER.compare(this , str);
1284            }
1285
1286            /**
1287             * Tests if two string regions are equal.
1288             * <p>
1289             * A substring of this <tt>String</tt> object is compared to a substring
1290             * of the argument other. The result is true if these substrings
1291             * represent identical character sequences. The substring of this
1292             * <tt>String</tt> object to be compared begins at index <tt>toffset</tt>
1293             * and has length <tt>len</tt>. The substring of other to be compared
1294             * begins at index <tt>ooffset</tt> and has length <tt>len</tt>. The
1295             * result is <tt>false</tt> if and only if at least one of the following
1296             * is true:
1297             * <ul><li><tt>toffset</tt> is negative.
1298             * <li><tt>ooffset</tt> is negative.
1299             * <li><tt>toffset+len</tt> is greater than the length of this
1300             * <tt>String</tt> object.
1301             * <li><tt>ooffset+len</tt> is greater than the length of the other
1302             * argument.
1303             * <li>There is some nonnegative integer <i>k</i> less than <tt>len</tt>
1304             * such that:
1305             * <tt>this.charAt(toffset+<i>k</i>)&nbsp;!=&nbsp;other.charAt(ooffset+<i>k</i>)</tt>
1306             * </ul>
1307             *
1308             * @param   toffset   the starting offset of the subregion in this string.
1309             * @param   other     the string argument.
1310             * @param   ooffset   the starting offset of the subregion in the string
1311             *                    argument.
1312             * @param   len       the number of characters to compare.
1313             * @return  <code>true</code> if the specified subregion of this string
1314             *          exactly matches the specified subregion of the string argument;
1315             *          <code>false</code> otherwise.
1316             */
1317            public boolean regionMatches(int toffset, String other,
1318                    int ooffset, int len) {
1319                char ta[] = value;
1320                int to = offset + toffset;
1321                char pa[] = other.value;
1322                int po = other.offset + ooffset;
1323                // Note: toffset, ooffset, or len might be near -1>>>1.
1324                if ((ooffset < 0) || (toffset < 0)
1325                        || (toffset > (long) count - len)
1326                        || (ooffset > (long) other.count - len)) {
1327                    return false;
1328                }
1329                while (len-- > 0) {
1330                    if (ta[to++] != pa[po++]) {
1331                        return false;
1332                    }
1333                }
1334                return true;
1335            }
1336
1337            /**
1338             * Tests if two string regions are equal.
1339             * <p>
1340             * A substring of this <tt>String</tt> object is compared to a substring
1341             * of the argument <tt>other</tt>. The result is <tt>true</tt> if these
1342             * substrings represent character sequences that are the same, ignoring
1343             * case if and only if <tt>ignoreCase</tt> is true. The substring of
1344             * this <tt>String</tt> object to be compared begins at index
1345             * <tt>toffset</tt> and has length <tt>len</tt>. The substring of
1346             * <tt>other</tt> to be compared begins at index <tt>ooffset</tt> and
1347             * has length <tt>len</tt>. The result is <tt>false</tt> if and only if
1348             * at least one of the following is true:
1349             * <ul><li><tt>toffset</tt> is negative.
1350             * <li><tt>ooffset</tt> is negative.
1351             * <li><tt>toffset+len</tt> is greater than the length of this
1352             * <tt>String</tt> object.
1353             * <li><tt>ooffset+len</tt> is greater than the length of the other
1354             * argument.
1355             * <li><tt>ignoreCase</tt> is <tt>false</tt> and there is some nonnegative
1356             * integer <i>k</i> less than <tt>len</tt> such that:
1357             * <blockquote><pre>
1358             * this.charAt(toffset+k) != other.charAt(ooffset+k)
1359             * </pre></blockquote>
1360             * <li><tt>ignoreCase</tt> is <tt>true</tt> and there is some nonnegative
1361             * integer <i>k</i> less than <tt>len</tt> such that:
1362             * <blockquote><pre>
1363             * Character.toLowerCase(this.charAt(toffset+k)) !=
1364                       Character.toLowerCase(other.charAt(ooffset+k))
1365             * </pre></blockquote>
1366             * and:
1367             * <blockquote><pre>
1368             * Character.toUpperCase(this.charAt(toffset+k)) !=
1369             *         Character.toUpperCase(other.charAt(ooffset+k))
1370             * </pre></blockquote>
1371             * </ul>
1372             *
1373             * @param   ignoreCase   if <code>true</code>, ignore case when comparing
1374             *                       characters.
1375             * @param   toffset      the starting offset of the subregion in this
1376             *                       string.
1377             * @param   other        the string argument.
1378             * @param   ooffset      the starting offset of the subregion in the string
1379             *                       argument.
1380             * @param   len          the number of characters to compare.
1381             * @return  <code>true</code> if the specified subregion of this string
1382             *          matches the specified subregion of the string argument;
1383             *          <code>false</code> otherwise. Whether the matching is exact
1384             *          or case insensitive depends on the <code>ignoreCase</code>
1385             *          argument.
1386             */
1387            public boolean regionMatches(boolean ignoreCase, int toffset,
1388                    String other, int ooffset, int len) {
1389                char ta[] = value;
1390                int to = offset + toffset;
1391                char pa[] = other.value;
1392                int po = other.offset + ooffset;
1393                // Note: toffset, ooffset, or len might be near -1>>>1.
1394                if ((ooffset < 0) || (toffset < 0)
1395                        || (toffset > (long) count - len)
1396                        || (ooffset > (long) other.count - len)) {
1397                    return false;
1398                }
1399                while (len-- > 0) {
1400                    char c1 = ta[to++];
1401                    char c2 = pa[po++];
1402                    if (c1 == c2) {
1403                        continue;
1404                    }
1405                    if (ignoreCase) {
1406                        // If characters don't match but case may be ignored,
1407                        // try converting both characters to uppercase.
1408                        // If the results match, then the comparison scan should
1409                        // continue.
1410                        char u1 = Character.toUpperCase(c1);
1411                        char u2 = Character.toUpperCase(c2);
1412                        if (u1 == u2) {
1413                            continue;
1414                        }
1415                        // Unfortunately, conversion to uppercase does not work properly
1416                        // for the Georgian alphabet, which has strange rules about case
1417                        // conversion.  So we need to make one last check before
1418                        // exiting.
1419                        if (Character.toLowerCase(u1) == Character
1420                                .toLowerCase(u2)) {
1421                            continue;
1422                        }
1423                    }
1424                    return false;
1425                }
1426                return true;
1427            }
1428
1429            /**
1430             * Tests if the substring of this string beginning at the
1431             * specified index starts with the specified prefix.
1432             *
1433             * @param   prefix    the prefix.
1434             * @param   toffset   where to begin looking in this string.
1435             * @return  <code>true</code> if the character sequence represented by the
1436             *          argument is a prefix of the substring of this object starting
1437             *          at index <code>toffset</code>; <code>false</code> otherwise.
1438             *          The result is <code>false</code> if <code>toffset</code> is
1439             *          negative or greater than the length of this
1440             *          <code>String</code> object; otherwise the result is the same
1441             *          as the result of the expression
1442             *          <pre>
1443             *          this.substring(toffset).startsWith(prefix)
1444             *          </pre>
1445             */
1446            public boolean startsWith(String prefix, int toffset) {
1447                char ta[] = value;
1448                int to = offset + toffset;
1449                char pa[] = prefix.value;
1450                int po = prefix.offset;
1451                int pc = prefix.count;
1452                // Note: toffset might be near -1>>>1.
1453                if ((toffset < 0) || (toffset > count - pc)) {
1454                    return false;
1455                }
1456                while (--pc >= 0) {
1457                    if (ta[to++] != pa[po++]) {
1458                        return false;
1459                    }
1460                }
1461                return true;
1462            }
1463
1464            /**
1465             * Tests if this string starts with the specified prefix.
1466             *
1467             * @param   prefix   the prefix.
1468             * @return  <code>true</code> if the character sequence represented by the
1469             *          argument is a prefix of the character sequence represented by
1470             *          this string; <code>false</code> otherwise.
1471             *          Note also that <code>true</code> will be returned if the
1472             *          argument is an empty string or is equal to this
1473             *          <code>String</code> object as determined by the
1474             *          {@link #equals(Object)} method.
1475             * @since   1. 0
1476             */
1477            public boolean startsWith(String prefix) {
1478                return startsWith(prefix, 0);
1479            }
1480
1481            /**
1482             * Tests if this string ends with the specified suffix.
1483             *
1484             * @param   suffix   the suffix.
1485             * @return  <code>true</code> if the character sequence represented by the
1486             *          argument is a suffix of the character sequence represented by
1487             *          this object; <code>false</code> otherwise. Note that the
1488             *          result will be <code>true</code> if the argument is the
1489             *          empty string or is equal to this <code>String</code> object
1490             *          as determined by the {@link #equals(Object)} method.
1491             */
1492            public boolean endsWith(String suffix) {
1493                return startsWith(suffix, count - suffix.count);
1494            }
1495
1496            /**
1497             * Returns a hash code for this string. The hash code for a
1498             * <code>String</code> object is computed as
1499             * <blockquote><pre>
1500             * s[0]*31^(n-1) + s[1]*31^(n-2) + ... + s[n-1]
1501             * </pre></blockquote>
1502             * using <code>int</code> arithmetic, where <code>s[i]</code> is the
1503             * <i>i</i>th character of the string, <code>n</code> is the length of
1504             * the string, and <code>^</code> indicates exponentiation.
1505             * (The hash value of the empty string is zero.)
1506             *
1507             * @return  a hash code value for this object.
1508             */
1509            public int hashCode() {
1510                int h = hash;
1511                if (h == 0) {
1512                    int off = offset;
1513                    char val[] = value;
1514                    int len = count;
1515
1516                    for (int i = 0; i < len; i++) {
1517                        h = 31 * h + val[off++];
1518                    }
1519                    hash = h;
1520                }
1521                return h;
1522            }
1523
1524            /**
1525             * Returns the index within this string of the first occurrence of
1526             * the specified character. If a character with value
1527             * <code>ch</code> occurs in the character sequence represented by
1528             * this <code>String</code> object, then the index (in Unicode
1529             * code units) of the first such occurrence is returned. For
1530             * values of <code>ch</code> in the range from 0 to 0xFFFF
1531             * (inclusive), this is the smallest value <i>k</i> such that:
1532             * <blockquote><pre>
1533             * this.charAt(<i>k</i>) == ch
1534             * </pre></blockquote>
1535             * is true. For other values of <code>ch</code>, it is the
1536             * smallest value <i>k</i> such that:
1537             * <blockquote><pre>
1538             * this.codePointAt(<i>k</i>) == ch
1539             * </pre></blockquote>
1540             * is true. In either case, if no such character occurs in this
1541             * string, then <code>-1</code> is returned.
1542             *
1543             * @param   ch   a character (Unicode code point).
1544             * @return  the index of the first occurrence of the character in the
1545             *          character sequence represented by this object, or
1546             *          <code>-1</code> if the character does not occur.
1547             */
1548            public int indexOf(int ch) {
1549                return indexOf(ch, 0);
1550            }
1551
1552            /**
1553             * Returns the index within this string of the first occurrence of the
1554             * specified character, starting the search at the specified index.
1555             * <p>
1556             * If a character with value <code>ch</code> occurs in the
1557             * character sequence represented by this <code>String</code>
1558             * object at an index no smaller than <code>fromIndex</code>, then
1559             * the index of the first such occurrence is returned. For values
1560             * of <code>ch</code> in the range from 0 to 0xFFFF (inclusive),
1561             * this is the smallest value <i>k</i> such that:
1562             * <blockquote><pre>
1563             * (this.charAt(<i>k</i>) == ch) && (<i>k</i> &gt;= fromIndex)
1564             * </pre></blockquote>
1565             * is true. For other values of <code>ch</code>, it is the
1566             * smallest value <i>k</i> such that:
1567             * <blockquote><pre>
1568             * (this.codePointAt(<i>k</i>) == ch) && (<i>k</i> &gt;= fromIndex)
1569             * </pre></blockquote>
1570             * is true. In either case, if no such character occurs in this
1571             * string at or after position <code>fromIndex</code>, then
1572             * <code>-1</code> is returned.
1573             *
1574             * <p>
1575             * There is no restriction on the value of <code>fromIndex</code>. If it
1576             * is negative, it has the same effect as if it were zero: this entire
1577             * string may be searched. If it is greater than the length of this
1578             * string, it has the same effect as if it were equal to the length of
1579             * this string: <code>-1</code> is returned.
1580             *
1581             * <p>All indices are specified in <code>char</code> values
1582             * (Unicode code units).
1583             *
1584             * @param   ch          a character (Unicode code point).
1585             * @param   fromIndex   the index to start the search from.
1586             * @return  the index of the first occurrence of the character in the
1587             *          character sequence represented by this object that is greater
1588             *          than or equal to <code>fromIndex</code>, or <code>-1</code>
1589             *          if the character does not occur.
1590             */
1591            public int indexOf(int ch, int fromIndex) {
1592                int max = offset + count;
1593                char v[] = value;
1594
1595                if (fromIndex < 0) {
1596                    fromIndex = 0;
1597                } else if (fromIndex >= count) {
1598                    // Note: fromIndex might be near -1>>>1.
1599                    return -1;
1600                }
1601
1602                int i = offset + fromIndex;
1603                if (ch < Character.MIN_SUPPLEMENTARY_CODE_POINT) {
1604                    // handle most cases here (ch is a BMP code point or a
1605                    // negative value (invalid code point))
1606                    for (; i < max; i++) {
1607                        if (v[i] == ch) {
1608                            return i - offset;
1609                        }
1610                    }
1611                    return -1;
1612                }
1613
1614                if (ch <= Character.MAX_CODE_POINT) {
1615                    // handle supplementary characters here
1616                    char[] surrogates = Character.toChars(ch);
1617                    for (; i < max; i++) {
1618                        if (v[i] == surrogates[0]) {
1619                            if (i + 1 == max) {
1620                                break;
1621                            }
1622                            if (v[i + 1] == surrogates[1]) {
1623                                return i - offset;
1624                            }
1625                        }
1626                    }
1627                }
1628                return -1;
1629            }
1630
1631            /**
1632             * Returns the index within this string of the last occurrence of
1633             * the specified character. For values of <code>ch</code> in the
1634             * range from 0 to 0xFFFF (inclusive), the index (in Unicode code
1635             * units) returned is the largest value <i>k</i> such that:
1636             * <blockquote><pre>
1637             * this.charAt(<i>k</i>) == ch
1638             * </pre></blockquote>
1639             * is true. For other values of <code>ch</code>, it is the
1640             * largest value <i>k</i> such that:
1641             * <blockquote><pre>
1642             * this.codePointAt(<i>k</i>) == ch
1643             * </pre></blockquote>
1644             * is true.  In either case, if no such character occurs in this
1645             * string, then <code>-1</code> is returned.  The
1646             * <code>String</code> is searched backwards starting at the last
1647             * character.
1648             *
1649             * @param   ch   a character (Unicode code point).
1650             * @return  the index of the last occurrence of the character in the
1651             *          character sequence represented by this object, or
1652             *          <code>-1</code> if the character does not occur.
1653             */
1654            public int lastIndexOf(int ch) {
1655                return lastIndexOf(ch, count - 1);
1656            }
1657
1658            /**
1659             * Returns the index within this string of the last occurrence of
1660             * the specified character, searching backward starting at the
1661             * specified index. For values of <code>ch</code> in the range
1662             * from 0 to 0xFFFF (inclusive), the index returned is the largest
1663             * value <i>k</i> such that:
1664             * <blockquote><pre>
1665             * (this.charAt(<i>k</i>) == ch) && (<i>k</i> &lt;= fromIndex)
1666             * </pre></blockquote>
1667             * is true. For other values of <code>ch</code>, it is the
1668             * largest value <i>k</i> such that:
1669             * <blockquote><pre>
1670             * (this.codePointAt(<i>k</i>) == ch) && (<i>k</i> &lt;= fromIndex)
1671             * </pre></blockquote>
1672             * is true. In either case, if no such character occurs in this
1673             * string at or before position <code>fromIndex</code>, then
1674             * <code>-1</code> is returned.
1675             *
1676             * <p>All indices are specified in <code>char</code> values
1677             * (Unicode code units).
1678             *
1679             * @param   ch          a character (Unicode code point).
1680             * @param   fromIndex   the index to start the search from. There is no
1681             *          restriction on the value of <code>fromIndex</code>. If it is
1682             *          greater than or equal to the length of this string, it has
1683             *          the same effect as if it were equal to one less than the
1684             *          length of this string: this entire string may be searched.
1685             *          If it is negative, it has the same effect as if it were -1:
1686             *          -1 is returned.
1687             * @return  the index of the last occurrence of the character in the
1688             *          character sequence represented by this object that is less
1689             *          than or equal to <code>fromIndex</code>, or <code>-1</code>
1690             *          if the character does not occur before that point.
1691             */
1692            public int lastIndexOf(int ch, int fromIndex) {
1693                int min = offset;
1694                char v[] = value;
1695
1696                int i = offset + ((fromIndex >= count) ? count - 1 : fromIndex);
1697
1698                if (ch < Character.MIN_SUPPLEMENTARY_CODE_POINT) {
1699                    // handle most cases here (ch is a BMP code point or a
1700                    // negative value (invalid code point))
1701                    for (; i >= min; i--) {
1702                        if (v[i] == ch) {
1703                            return i - offset;
1704                        }
1705                    }
1706                    return -1;
1707                }
1708
1709                int max = offset + count;
1710                if (ch <= Character.MAX_CODE_POINT) {
1711                    // handle supplementary characters here
1712                    char[] surrogates = Character.toChars(ch);
1713                    for (; i >= min; i--) {
1714                        if (v[i] == surrogates[0]) {
1715                            if (i + 1 == max) {
1716                                break;
1717                            }
1718                            if (v[i + 1] == surrogates[1]) {
1719                                return i - offset;
1720                            }
1721                        }
1722                    }
1723                }
1724                return -1;
1725            }
1726
1727            /**
1728             * Returns the index within this string of the first occurrence of the
1729             * specified substring. The integer returned is the smallest value
1730             * <i>k</i> such that:
1731             * <blockquote><pre>
1732             * this.startsWith(str, <i>k</i>)
1733             * </pre></blockquote>
1734             * is <code>true</code>.
1735             *
1736             * @param   str   any string.
1737             * @return  if the string argument occurs as a substring within this
1738             *          object, then the index of the first character of the first
1739             *          such substring is returned; if it does not occur as a
1740             *          substring, <code>-1</code> is returned.
1741             */
1742            public int indexOf(String str) {
1743                return indexOf(str, 0);
1744            }
1745
1746            /**
1747             * Returns the index within this string of the first occurrence of the
1748             * specified substring, starting at the specified index.  The integer
1749             * returned is the smallest value <tt>k</tt> for which:
1750             * <blockquote><pre>
1751             *     k &gt;= Math.min(fromIndex, this.length()) && this.startsWith(str, k)
1752             * </pre></blockquote>
1753             * If no such value of <i>k</i> exists, then -1 is returned.
1754             *
1755             * @param   str         the substring for which to search.
1756             * @param   fromIndex   the index from which to start the search.
1757             * @return  the index within this string of the first occurrence of the
1758             *          specified substring, starting at the specified index.
1759             */
1760            public int indexOf(String str, int fromIndex) {
1761                return indexOf(value, offset, count, str.value, str.offset,
1762                        str.count, fromIndex);
1763            }
1764
1765            /**
1766             * Code shared by String and StringBuffer to do searches. The
1767             * source is the character array being searched, and the target
1768             * is the string being searched for.
1769             *
1770             * @param   source       the characters being searched.
1771             * @param   sourceOffset offset of the source string.
1772             * @param   sourceCount  count of the source string.
1773             * @param   target       the characters being searched for.
1774             * @param   targetOffset offset of the target string.
1775             * @param   targetCount  count of the target string.
1776             * @param   fromIndex    the index to begin searching from.
1777             */
1778            static int indexOf(char[] source, int sourceOffset,
1779                    int sourceCount, char[] target, int targetOffset,
1780                    int targetCount, int fromIndex) {
1781                if (fromIndex >= sourceCount) {
1782                    return (targetCount == 0 ? sourceCount : -1);
1783                }
1784                if (fromIndex < 0) {
1785                    fromIndex = 0;
1786                }
1787                if (targetCount == 0) {
1788                    return fromIndex;
1789                }
1790
1791                char first = target[targetOffset];
1792                int max = sourceOffset + (sourceCount - targetCount);
1793
1794                for (int i = sourceOffset + fromIndex; i <= max; i++) {
1795                    /* Look for first character. */
1796                    if (source[i] != first) {
1797                        while (++i <= max && source[i] != first)
1798                            ;
1799                    }
1800
1801                    /* Found first character, now look at the rest of v2 */
1802                    if (i <= max) {
1803                        int j = i + 1;
1804                        int end = j + targetCount - 1;
1805                        for (int k = targetOffset + 1; j < end
1806                                && source[j] == target[k]; j++, k++)
1807                            ;
1808
1809                        if (j == end) {
1810                            /* Found whole string. */
1811                            return i - sourceOffset;
1812                        }
1813                    }
1814                }
1815                return -1;
1816            }
1817
1818            /**
1819             * Returns the index within this string of the rightmost occurrence
1820             * of the specified substring.  The rightmost empty string "" is
1821             * considered to occur at the index value <code>this.length()</code>.
1822             * The returned index is the largest value <i>k</i> such that
1823             * <blockquote><pre>
1824             * this.startsWith(str, k)
1825             * </pre></blockquote>
1826             * is true.
1827             *
1828             * @param   str   the substring to search for.
1829             * @return  if the string argument occurs one or more times as a substring
1830             *          within this object, then the index of the first character of
1831             *          the last such substring is returned. If it does not occur as
1832             *          a substring, <code>-1</code> is returned.
1833             */
1834            public int lastIndexOf(String str) {
1835                return lastIndexOf(str, count);
1836            }
1837
1838            /**
1839             * Returns the index within this string of the last occurrence of the
1840             * specified substring, searching backward starting at the specified index.
1841             * The integer returned is the largest value <i>k</i> such that:
1842             * <blockquote><pre>
1843             *     k &lt;= Math.min(fromIndex, this.length()) && this.startsWith(str, k)
1844             * </pre></blockquote>
1845             * If no such value of <i>k</i> exists, then -1 is returned.
1846             *
1847             * @param   str         the substring to search for.
1848             * @param   fromIndex   the index to start the search from.
1849             * @return  the index within this string of the last occurrence of the
1850             *          specified substring.
1851             */
1852            public int lastIndexOf(String str, int fromIndex) {
1853                return lastIndexOf(value, offset, count, str.value, str.offset,
1854                        str.count, fromIndex);
1855            }
1856
1857            /**
1858             * Code shared by String and StringBuffer to do searches. The
1859             * source is the character array being searched, and the target
1860             * is the string being searched for.
1861             *
1862             * @param   source       the characters being searched.
1863             * @param   sourceOffset offset of the source string.
1864             * @param   sourceCount  count of the source string.
1865             * @param   target       the characters being searched for.
1866             * @param   targetOffset offset of the target string.
1867             * @param   targetCount  count of the target string.
1868             * @param   fromIndex    the index to begin searching from.
1869             */
1870            static int lastIndexOf(char[] source, int sourceOffset,
1871                    int sourceCount, char[] target, int targetOffset,
1872                    int targetCount, int fromIndex) {
1873                /*
1874                 * Check arguments; return immediately where possible. For
1875                 * consistency, don't check for null str.
1876                 */
1877                int rightIndex = sourceCount - targetCount;
1878                if (fromIndex < 0) {
1879                    return -1;
1880                }
1881                if (fromIndex > rightIndex) {
1882                    fromIndex = rightIndex;
1883                }
1884                /* Empty string always matches. */
1885                if (targetCount == 0) {
1886                    return fromIndex;
1887                }
1888
1889                int strLastIndex = targetOffset + targetCount - 1;
1890                char strLastChar = target[strLastIndex];
1891                int min = sourceOffset + targetCount - 1;
1892                int i = min + fromIndex;
1893
1894                startSearchForLastChar: while (true) {
1895                    while (i >= min && source[i] != strLastChar) {
1896                        i--;
1897                    }
1898                    if (i < min) {
1899                        return -1;
1900                    }
1901                    int j = i - 1;
1902                    int start = j - (targetCount - 1);
1903                    int k = strLastIndex - 1;
1904
1905                    while (j > start) {
1906                        if (source[j--] != target[k--]) {
1907                            i--;
1908                            continue startSearchForLastChar;
1909                        }
1910                    }
1911                    return start - sourceOffset + 1;
1912                }
1913            }
1914
1915            /**
1916             * Returns a new string that is a substring of this string. The
1917             * substring begins with the character at the specified index and
1918             * extends to the end of this string. <p>
1919             * Examples:
1920             * <blockquote><pre>
1921             * "unhappy".substring(2) returns "happy"
1922             * "Harbison".substring(3) returns "bison"
1923             * "emptiness".substring(9) returns "" (an empty string)
1924             * </pre></blockquote>
1925             *
1926             * @param      beginIndex   the beginning index, inclusive.
1927             * @return     the specified substring.
1928             * @exception  IndexOutOfBoundsException  if
1929             *             <code>beginIndex</code> is negative or larger than the
1930             *             length of this <code>String</code> object.
1931             */
1932            public String substring(int beginIndex) {
1933                return substring(beginIndex, count);
1934            }
1935
1936            /**
1937             * Returns a new string that is a substring of this string. The
1938             * substring begins at the specified <code>beginIndex</code> and
1939             * extends to the character at index <code>endIndex - 1</code>.
1940             * Thus the length of the substring is <code>endIndex-beginIndex</code>.
1941             * <p>
1942             * Examples:
1943             * <blockquote><pre>
1944             * "hamburger".substring(4, 8) returns "urge"
1945             * "smiles".substring(1, 5) returns "mile"
1946             * </pre></blockquote>
1947             *
1948             * @param      beginIndex   the beginning index, inclusive.
1949             * @param      endIndex     the ending index, exclusive.
1950             * @return     the specified substring.
1951             * @exception  IndexOutOfBoundsException  if the
1952             *             <code>beginIndex</code> is negative, or
1953             *             <code>endIndex</code> is larger than the length of
1954             *             this <code>String</code> object, or
1955             *             <code>beginIndex</code> is larger than
1956             *             <code>endIndex</code>.
1957             */
1958            public String substring(int beginIndex, int endIndex) {
1959                if (beginIndex < 0) {
1960                    throw new StringIndexOutOfBoundsException(beginIndex);
1961                }
1962                if (endIndex > count) {
1963                    throw new StringIndexOutOfBoundsException(endIndex);
1964                }
1965                if (beginIndex > endIndex) {
1966                    throw new StringIndexOutOfBoundsException(endIndex
1967                            - beginIndex);
1968                }
1969                return ((beginIndex == 0) && (endIndex == count)) ? this 
1970                        : new String(offset + beginIndex,
1971                                endIndex - beginIndex, value);
1972            }
1973
1974            /**
1975             * Returns a new character sequence that is a subsequence of this sequence.
1976             *
1977             * <p> An invocation of this method of the form
1978             *
1979             * <blockquote><pre>
1980             * str.subSequence(begin,&nbsp;end)</pre></blockquote>
1981             *
1982             * behaves in exactly the same way as the invocation
1983             *
1984             * <blockquote><pre>
1985             * str.substring(begin,&nbsp;end)</pre></blockquote>
1986             *
1987             * This method is defined so that the <tt>String</tt> class can implement
1988             * the {@link CharSequence} interface. </p>
1989             *
1990             * @param      beginIndex   the begin index, inclusive.
1991             * @param      endIndex     the end index, exclusive.
1992             * @return     the specified subsequence.
1993             *
1994             * @throws  IndexOutOfBoundsException
1995             *          if <tt>beginIndex</tt> or <tt>endIndex</tt> are negative,
1996             *          if <tt>endIndex</tt> is greater than <tt>length()</tt>,
1997             *          or if <tt>beginIndex</tt> is greater than <tt>startIndex</tt>
1998             *
1999             * @since 1.4
2000             * @spec JSR-51
2001             */
2002            public CharSequence subSequence(int beginIndex, int endIndex) {
2003                return this .substring(beginIndex, endIndex);
2004            }
2005
2006            /**
2007             * Concatenates the specified string to the end of this string.
2008             * <p>
2009             * If the length of the argument string is <code>0</code>, then this
2010             * <code>String</code> object is returned. Otherwise, a new
2011             * <code>String</code> object is created, representing a character
2012             * sequence that is the concatenation of the character sequence
2013             * represented by this <code>String</code> object and the character
2014             * sequence represented by the argument string.<p>
2015             * Examples:
2016             * <blockquote><pre>
2017             * "cares".concat("s") returns "caress"
2018             * "to".concat("get").concat("her") returns "together"
2019             * </pre></blockquote>
2020             *
2021             * @param   str   the <code>String</code> that is concatenated to the end
2022             *                of this <code>String</code>.
2023             * @return  a string that represents the concatenation of this object's
2024             *          characters followed by the string argument's characters.
2025             */
2026            public String concat(String str) {
2027                int otherLen = str.length();
2028                if (otherLen == 0) {
2029                    return this ;
2030                }
2031                char buf[] = new char[count + otherLen];
2032                getChars(0, count, buf, 0);
2033                str.getChars(0, otherLen, buf, count);
2034                return new String(0, count + otherLen, buf);
2035            }
2036
2037            /**
2038             * Returns a new string resulting from replacing all occurrences of
2039             * <code>oldChar</code> in this string with <code>newChar</code>.
2040             * <p>
2041             * If the character <code>oldChar</code> does not occur in the
2042             * character sequence represented by this <code>String</code> object,
2043             * then a reference to this <code>String</code> object is returned.
2044             * Otherwise, a new <code>String</code> object is created that
2045             * represents a character sequence identical to the character sequence
2046             * represented by this <code>String</code> object, except that every
2047             * occurrence of <code>oldChar</code> is replaced by an occurrence
2048             * of <code>newChar</code>.
2049             * <p>
2050             * Examples:
2051             * <blockquote><pre>
2052             * "mesquite in your cellar".replace('e', 'o')
2053             *         returns "mosquito in your collar"
2054             * "the war of baronets".replace('r', 'y')
2055             *         returns "the way of bayonets"
2056             * "sparring with a purple porpoise".replace('p', 't')
2057             *         returns "starring with a turtle tortoise"
2058             * "JonL".replace('q', 'x') returns "JonL" (no change)
2059             * </pre></blockquote>
2060             *
2061             * @param   oldChar   the old character.
2062             * @param   newChar   the new character.
2063             * @return  a string derived from this string by replacing every
2064             *          occurrence of <code>oldChar</code> with <code>newChar</code>.
2065             */
2066            public String replace(char oldChar, char newChar) {
2067                if (oldChar != newChar) {
2068                    int len = count;
2069                    int i = -1;
2070                    char[] val = value; /* avoid getfield opcode */
2071                    int off = offset; /* avoid getfield opcode */
2072
2073                    while (++i < len) {
2074                        if (val[off + i] == oldChar) {
2075                            break;
2076                        }
2077                    }
2078                    if (i < len) {
2079                        char buf[] = new char[len];
2080                        for (int j = 0; j < i; j++) {
2081                            buf[j] = val[off + j];
2082                        }
2083                        while (i < len) {
2084                            char c = val[off + i];
2085                            buf[i] = (c == oldChar) ? newChar : c;
2086                            i++;
2087                        }
2088                        return new String(0, len, buf);
2089                    }
2090                }
2091                return this ;
2092            }
2093
2094            /**
2095             * Tells whether or not this string matches the given <a
2096             * href="../util/regex/Pattern.html#sum">regular expression</a>.
2097             *
2098             * <p> An invocation of this method of the form
2099             * <i>str</i><tt>.matches(</tt><i>regex</i><tt>)</tt> yields exactly the
2100             * same result as the expression
2101             *
2102             * <blockquote><tt> {@link java.util.regex.Pattern}.{@link
2103             * java.util.regex.Pattern#matches(String,CharSequence)
2104             * matches}(</tt><i>regex</i><tt>,</tt> <i>str</i><tt>)</tt></blockquote>
2105             *
2106             * @param   regex
2107             *          the regular expression to which this string is to be matched
2108             *
2109             * @return  <tt>true</tt> if, and only if, this string matches the
2110             *          given regular expression
2111             *
2112             * @throws  PatternSyntaxException
2113             *          if the regular expression's syntax is invalid
2114             *
2115             * @see java.util.regex.Pattern
2116             *
2117             * @since 1.4
2118             * @spec JSR-51
2119             */
2120            public boolean matches(String regex) {
2121                return Pattern.matches(regex, this );
2122            }
2123
2124            /**
2125             * Returns true if and only if this string contains the specified
2126             * sequence of char values.
2127             *
2128             * @param s the sequence to search for
2129             * @return true if this string contains <code>s</code>, false otherwise
2130             * @throws NullPointerException if <code>s</code> is <code>null</code>
2131             * @since 1.5
2132             */
2133            public boolean contains(CharSequence s) {
2134                return indexOf(s.toString()) > -1;
2135            }
2136
2137            /**
2138             * Replaces the first substring of this string that matches the given <a
2139             * href="../util/regex/Pattern.html#sum">regular expression</a> with the
2140             * given replacement.
2141             *
2142             * <p> An invocation of this method of the form
2143             * <i>str</i><tt>.replaceFirst(</tt><i>regex</i><tt>,</tt> <i>repl</i><tt>)</tt>
2144             * yields exactly the same result as the expression
2145             *
2146             * <blockquote><tt>
2147             * {@link java.util.regex.Pattern}.{@link java.util.regex.Pattern#compile
2148             * compile}(</tt><i>regex</i><tt>).{@link
2149             * java.util.regex.Pattern#matcher(java.lang.CharSequence)
2150             * matcher}(</tt><i>str</i><tt>).{@link java.util.regex.Matcher#replaceFirst
2151             * replaceFirst}(</tt><i>repl</i><tt>)</tt></blockquote>
2152             *
2153             *<p>
2154             * Note that backslashes (<tt>\</tt>) and dollar signs (<tt>$</tt>) in the
2155             * replacement string may cause the results to be different than if it were
2156             * being treated as a literal replacement string; see
2157             * {@link java.util.regex.Matcher#replaceFirst}.
2158             * Use {@link java.util.regex.Matcher#quoteReplacement} to suppress the special
2159             * meaning of these characters, if desired.
2160             *
2161             * @param   regex
2162             *          the regular expression to which this string is to be matched
2163             * @param   replacement
2164             *          the string to be substituted for the first match
2165             *
2166             * @return  The resulting <tt>String</tt>
2167             *
2168             * @throws  PatternSyntaxException
2169             *          if the regular expression's syntax is invalid
2170             *
2171             * @see java.util.regex.Pattern
2172             *
2173             * @since 1.4
2174             * @spec JSR-51
2175             */
2176            public String replaceFirst(String regex, String replacement) {
2177                return Pattern.compile(regex).matcher(this ).replaceFirst(
2178                        replacement);
2179            }
2180
2181            /**
2182             * Replaces each substring of this string that matches the given <a
2183             * href="../util/regex/Pattern.html#sum">regular expression</a> with the
2184             * given replacement.
2185             *
2186             * <p> An invocation of this method of the form
2187             * <i>str</i><tt>.replaceAll(</tt><i>regex</i><tt>,</tt> <i>repl</i><tt>)</tt>
2188             * yields exactly the same result as the expression
2189             *
2190             * <blockquote><tt>
2191             * {@link java.util.regex.Pattern}.{@link java.util.regex.Pattern#compile
2192             * compile}(</tt><i>regex</i><tt>).{@link
2193             * java.util.regex.Pattern#matcher(java.lang.CharSequence)
2194             * matcher}(</tt><i>str</i><tt>).{@link java.util.regex.Matcher#replaceAll
2195             * replaceAll}(</tt><i>repl</i><tt>)</tt></blockquote>
2196             *
2197             *<p>
2198             * Note that backslashes (<tt>\</tt>) and dollar signs (<tt>$</tt>) in the
2199             * replacement string may cause the results to be different than if it were
2200             * being treated as a literal replacement string; see
2201             * {@link java.util.regex.Matcher#replaceAll Matcher.replaceAll}.
2202             * Use {@link java.util.regex.Matcher#quoteReplacement} to suppress the special
2203             * meaning of these characters, if desired.
2204             *
2205             * @param   regex
2206             *          the regular expression to which this string is to be matched
2207             * @param   replacement
2208             *          the string to be substituted for each match
2209             *
2210             * @return  The resulting <tt>String</tt>
2211             *
2212             * @throws  PatternSyntaxException
2213             *          if the regular expression's syntax is invalid
2214             *
2215             * @see java.util.regex.Pattern
2216             *
2217             * @since 1.4
2218             * @spec JSR-51
2219             */
2220            public String replaceAll(String regex, String replacement) {
2221                return Pattern.compile(regex).matcher(this ).replaceAll(
2222                        replacement);
2223            }
2224
2225            /**
2226             * Replaces each substring of this string that matches the literal target
2227             * sequence with the specified literal replacement sequence. The
2228             * replacement proceeds from the beginning of the string to the end, for
2229             * example, replacing "aa" with "b" in the string "aaa" will result in
2230             * "ba" rather than "ab".
2231             *
2232             * @param  target The sequence of char values to be replaced
2233             * @param  replacement The replacement sequence of char values
2234             * @return  The resulting string
2235             * @throws NullPointerException if <code>target</code> or
2236             *         <code>replacement</code> is <code>null</code>.
2237             * @since 1.5
2238             */
2239            public String replace(CharSequence target, CharSequence replacement) {
2240                return Pattern.compile(target.toString(), Pattern.LITERAL)
2241                        .matcher(this ).replaceAll(
2242                                Matcher
2243                                        .quoteReplacement(replacement
2244                                                .toString()));
2245            }
2246
2247            /**
2248             * Splits this string around matches of the given
2249             * <a href="../util/regex/Pattern.html#sum">regular expression</a>.
2250             *
2251             * <p> The array returned by this method contains each substring of this
2252             * string that is terminated by another substring that matches the given
2253             * expression or is terminated by the end of the string.  The substrings in
2254             * the array are in the order in which they occur in this string.  If the
2255             * expression does not match any part of the input then the resulting array
2256             * has just one element, namely this string.
2257             *
2258             * <p> The <tt>limit</tt> parameter controls the number of times the
2259             * pattern is applied and therefore affects the length of the resulting
2260             * array.  If the limit <i>n</i> is greater than zero then the pattern
2261             * will be applied at most <i>n</i>&nbsp;-&nbsp;1 times, the array's
2262             * length will be no greater than <i>n</i>, and the array's last entry
2263             * will contain all input beyond the last matched delimiter.  If <i>n</i>
2264             * is non-positive then the pattern will be applied as many times as
2265             * possible and the array can have any length.  If <i>n</i> is zero then
2266             * the pattern will be applied as many times as possible, the array can
2267             * have any length, and trailing empty strings will be discarded.
2268             *
2269             * <p> The string <tt>"boo:and:foo"</tt>, for example, yields the
2270             * following results with these parameters:
2271             *
2272             * <blockquote><table cellpadding=1 cellspacing=0 summary="Split example showing regex, limit, and result">
2273             * <tr>
2274             *     <th>Regex</th>
2275             *     <th>Limit</th>
2276             *     <th>Result</th>
2277             * </tr>
2278             * <tr><td align=center>:</td>
2279             *     <td align=center>2</td>
2280             *     <td><tt>{ "boo", "and:foo" }</tt></td></tr>
2281             * <tr><td align=center>:</td>
2282             *     <td align=center>5</td>
2283             *     <td><tt>{ "boo", "and", "foo" }</tt></td></tr>
2284             * <tr><td align=center>:</td>
2285             *     <td align=center>-2</td>
2286             *     <td><tt>{ "boo", "and", "foo" }</tt></td></tr>
2287             * <tr><td align=center>o</td>
2288             *     <td align=center>5</td>
2289             *     <td><tt>{ "b", "", ":and:f", "", "" }</tt></td></tr>
2290             * <tr><td align=center>o</td>
2291             *     <td align=center>-2</td>
2292             *     <td><tt>{ "b", "", ":and:f", "", "" }</tt></td></tr>
2293             * <tr><td align=center>o</td>
2294             *     <td align=center>0</td>
2295             *     <td><tt>{ "b", "", ":and:f" }</tt></td></tr>
2296             * </table></blockquote>
2297             *
2298             * <p> An invocation of this method of the form
2299             * <i>str.</i><tt>split(</tt><i>regex</i><tt>,</tt>&nbsp;<i>n</i><tt>)</tt>
2300             * yields the same result as the expression
2301             *
2302             * <blockquote>
2303             * {@link java.util.regex.Pattern}.{@link java.util.regex.Pattern#compile
2304             * compile}<tt>(</tt><i>regex</i><tt>)</tt>.{@link
2305             * java.util.regex.Pattern#split(java.lang.CharSequence,int)
2306             * split}<tt>(</tt><i>str</i><tt>,</tt>&nbsp;<i>n</i><tt>)</tt>
2307             * </blockquote>
2308             *
2309             *
2310             * @param  regex
2311             *         the delimiting regular expression
2312             *
2313             * @param  limit
2314             *         the result threshold, as described above
2315             *
2316             * @return  the array of strings computed by splitting this string
2317             *          around matches of the given regular expression
2318             *
2319             * @throws  PatternSyntaxException
2320             *          if the regular expression's syntax is invalid
2321             *
2322             * @see java.util.regex.Pattern
2323             *
2324             * @since 1.4
2325             * @spec JSR-51
2326             */
2327            public String[] split(String regex, int limit) {
2328                return Pattern.compile(regex).split(this , limit);
2329            }
2330
2331            /**
2332             * Splits this string around matches of the given <a
2333             * href="../util/regex/Pattern.html#sum">regular expression</a>.
2334             *
2335             * <p> This method works as if by invoking the two-argument {@link
2336             * #split(String, int) split} method with the given expression and a limit
2337             * argument of zero.  Trailing empty strings are therefore not included in
2338             * the resulting array.
2339             *
2340             * <p> The string <tt>"boo:and:foo"</tt>, for example, yields the following
2341             * results with these expressions:
2342             *
2343             * <blockquote><table cellpadding=1 cellspacing=0 summary="Split examples showing regex and result">
2344             * <tr>
2345             *  <th>Regex</th>
2346             *  <th>Result</th>
2347             * </tr>
2348             * <tr><td align=center>:</td>
2349             *     <td><tt>{ "boo", "and", "foo" }</tt></td></tr>
2350             * <tr><td align=center>o</td>
2351             *     <td><tt>{ "b", "", ":and:f" }</tt></td></tr>
2352             * </table></blockquote>
2353             *
2354             *
2355             * @param  regex
2356             *         the delimiting regular expression
2357             *
2358             * @return  the array of strings computed by splitting this string
2359             *          around matches of the given regular expression
2360             *
2361             * @throws  PatternSyntaxException
2362             *          if the regular expression's syntax is invalid
2363             *
2364             * @see java.util.regex.Pattern
2365             *
2366             * @since 1.4
2367             * @spec JSR-51
2368             */
2369            public String[] split(String regex) {
2370                return split(regex, 0);
2371            }
2372
2373            /**
2374             * Converts all of the characters in this <code>String</code> to lower
2375             * case using the rules of the given <code>Locale</code>.  Case mapping is based
2376             * on the Unicode Standard version specified by the {@link java.lang.Character Character}
2377             * class. Since case mappings are not always 1:1 char mappings, the resulting
2378             * <code>String</code> may be a different length than the original <code>String</code>.
2379             * <p>
2380             * Examples of lowercase  mappings are in the following table:
2381             * <table border="1" summary="Lowercase mapping examples showing language code of locale, upper case, lower case, and description">
2382             * <tr>
2383             *   <th>Language Code of Locale</th>
2384             *   <th>Upper Case</th>
2385             *   <th>Lower Case</th>
2386             *   <th>Description</th>
2387             * </tr>
2388             * <tr>
2389             *   <td>tr (Turkish)</td>
2390             *   <td>&#92;u0130</td>
2391             *   <td>&#92;u0069</td>
2392             *   <td>capital letter I with dot above -&gt; small letter i</td>
2393             * </tr>
2394             * <tr>
2395             *   <td>tr (Turkish)</td>
2396             *   <td>&#92;u0049</td>
2397             *   <td>&#92;u0131</td>
2398             *   <td>capital letter I -&gt; small letter dotless i </td>
2399             * </tr>
2400             * <tr>
2401             *   <td>(all)</td>
2402             *   <td>French Fries</td>
2403             *   <td>french fries</td>
2404             *   <td>lowercased all chars in String</td>
2405             * </tr>
2406             * <tr>
2407             *   <td>(all)</td>
2408             *   <td><img src="doc-files/capiota.gif" alt="capiota"><img src="doc-files/capchi.gif" alt="capchi">
2409             *       <img src="doc-files/captheta.gif" alt="captheta"><img src="doc-files/capupsil.gif" alt="capupsil">
2410             *       <img src="doc-files/capsigma.gif" alt="capsigma"></td>
2411             *   <td><img src="doc-files/iota.gif" alt="iota"><img src="doc-files/chi.gif" alt="chi">
2412             *       <img src="doc-files/theta.gif" alt="theta"><img src="doc-files/upsilon.gif" alt="upsilon">
2413             *       <img src="doc-files/sigma1.gif" alt="sigma"></td>
2414             *   <td>lowercased all chars in String</td>
2415             * </tr>
2416             * </table>
2417             *
2418             * @param locale use the case transformation rules for this locale
2419             * @return the <code>String</code>, converted to lowercase.
2420             * @see     java.lang.String#toLowerCase()
2421             * @see     java.lang.String#toUpperCase()
2422             * @see     java.lang.String#toUpperCase(Locale)
2423             * @since   1.1
2424             */
2425            public String toLowerCase(Locale locale) {
2426                if (locale == null) {
2427                    throw new NullPointerException();
2428                }
2429
2430                int firstUpper;
2431
2432                /* Now check if there are any characters that need to be changed. */
2433                scan: {
2434                    for (firstUpper = 0; firstUpper < count;) {
2435                        char c = value[offset + firstUpper];
2436                        if ((c >= Character.MIN_HIGH_SURROGATE)
2437                                && (c <= Character.MAX_HIGH_SURROGATE)) {
2438                            int supplChar = codePointAt(firstUpper);
2439                            if (supplChar != Character.toLowerCase(supplChar)) {
2440                                break scan;
2441                            }
2442                            firstUpper += Character.charCount(supplChar);
2443                        } else {
2444                            if (c != Character.toLowerCase(c)) {
2445                                break scan;
2446                            }
2447                            firstUpper++;
2448                        }
2449                    }
2450                    return this ;
2451                }
2452
2453                char[] result = new char[count];
2454                int resultOffset = 0; /* result may grow, so i+resultOffset
2455                 * is the write location in result */
2456
2457                /* Just copy the first few lowerCase characters. */
2458                System.arraycopy(value, offset, result, 0, firstUpper);
2459
2460                String lang = locale.getLanguage();
2461                boolean localeDependent = (lang == "tr" || lang == "az" || lang == "lt");
2462                char[] lowerCharArray;
2463                int lowerChar;
2464                int srcChar;
2465                int srcCount;
2466                for (int i = firstUpper; i < count; i += srcCount) {
2467                    srcChar = (int) value[offset + i];
2468                    if ((char) srcChar >= Character.MIN_HIGH_SURROGATE
2469                            && (char) srcChar <= Character.MAX_HIGH_SURROGATE) {
2470                        srcChar = codePointAt(i);
2471                        srcCount = Character.charCount(srcChar);
2472                    } else {
2473                        srcCount = 1;
2474                    }
2475                    if (localeDependent || srcChar == '\u03A3') { // GREEK CAPITAL LETTER SIGMA
2476                        lowerChar = ConditionalSpecialCasing.toLowerCaseEx(
2477                                this , i, locale);
2478                    } else {
2479                        lowerChar = Character.toLowerCase(srcChar);
2480                    }
2481                    if ((lowerChar == Character.ERROR)
2482                            || (lowerChar >= Character.MIN_SUPPLEMENTARY_CODE_POINT)) {
2483                        if (lowerChar == Character.ERROR) {
2484                            lowerCharArray = ConditionalSpecialCasing
2485                                    .toLowerCaseCharArray(this , i, locale);
2486                        } else if (srcCount == 2) {
2487                            resultOffset += Character.toChars(lowerChar,
2488                                    result, i + resultOffset)
2489                                    - srcCount;
2490                            continue;
2491                        } else {
2492                            lowerCharArray = Character.toChars(lowerChar);
2493                        }
2494
2495                        /* Grow result if needed */
2496                        int mapLen = lowerCharArray.length;
2497                        if (mapLen > srcCount) {
2498                            char[] result2 = new char[result.length + mapLen
2499                                    - srcCount];
2500                            System.arraycopy(result, 0, result2, 0, i
2501                                    + resultOffset);
2502                            result = result2;
2503                        }
2504                        for (int x = 0; x < mapLen; ++x) {
2505                            result[i + resultOffset + x] = lowerCharArray[x];
2506                        }
2507                        resultOffset += (mapLen - srcCount);
2508                    } else {
2509                        result[i + resultOffset] = (char) lowerChar;
2510                    }
2511                }
2512                return new String(0, count + resultOffset, result);
2513            }
2514
2515            /**
2516             * Converts all of the characters in this <code>String</code> to lower
2517             * case using the rules of the default locale. This is equivalent to calling
2518             * <code>toLowerCase(Locale.getDefault())</code>.
2519             * <p>
2520             * <b>Note:</b> This method is locale sensitive, and may produce unexpected
2521             * results if used for strings that are intended to be interpreted locale
2522             * independently.
2523             * Examples are programming language identifiers, protocol keys, and HTML
2524             * tags.
2525             * For instance, <code>"TITLE".toLowerCase()</code> in a Turkish locale
2526             * returns <code>"t\u0131tle"</code>, where '\u0131' is the LATIN SMALL
2527             * LETTER DOTLESS I character.
2528             * To obtain correct results for locale insensitive strings, use
2529             * <code>toLowerCase(Locale.ENGLISH)</code>.
2530             * <p>
2531             * @return  the <code>String</code>, converted to lowercase.
2532             * @see     java.lang.String#toLowerCase(Locale)
2533             */
2534            public String toLowerCase() {
2535                return toLowerCase(Locale.getDefault());
2536            }
2537
2538            /**
2539             * Converts all of the characters in this <code>String</code> to upper
2540             * case using the rules of the given <code>Locale</code>. Case mapping is based
2541             * on the Unicode Standard version specified by the {@link java.lang.Character Character}
2542             * class. Since case mappings are not always 1:1 char mappings, the resulting
2543             * <code>String</code> may be a different length than the original <code>String</code>.
2544             * <p>
2545             * Examples of locale-sensitive and 1:M case mappings are in the following table.
2546             * <p>
2547             * <table border="1" summary="Examples of locale-sensitive and 1:M case mappings. Shows Language code of locale, lower case, upper case, and description.">
2548             * <tr>
2549             *   <th>Language Code of Locale</th>
2550             *   <th>Lower Case</th>
2551             *   <th>Upper Case</th>
2552             *   <th>Description</th>
2553             * </tr>
2554             * <tr>
2555             *   <td>tr (Turkish)</td>
2556             *   <td>&#92;u0069</td>
2557             *   <td>&#92;u0130</td>
2558             *   <td>small letter i -&gt; capital letter I with dot above</td>
2559             * </tr>
2560             * <tr>
2561             *   <td>tr (Turkish)</td>
2562             *   <td>&#92;u0131</td>
2563             *   <td>&#92;u0049</td>
2564             *   <td>small letter dotless i -&gt; capital letter I</td>
2565             * </tr>
2566             * <tr>
2567             *   <td>(all)</td>
2568             *   <td>&#92;u00df</td>
2569             *   <td>&#92;u0053 &#92;u0053</td>
2570             *   <td>small letter sharp s -&gt; two letters: SS</td>
2571             * </tr>
2572             * <tr>
2573             *   <td>(all)</td>
2574             *   <td>Fahrvergn&uuml;gen</td>
2575             *   <td>FAHRVERGN&Uuml;GEN</td>
2576             *   <td></td>
2577             * </tr>
2578             * </table>
2579             * @param locale use the case transformation rules for this locale
2580             * @return the <code>String</code>, converted to uppercase.
2581             * @see     java.lang.String#toUpperCase()
2582             * @see     java.lang.String#toLowerCase()
2583             * @see     java.lang.String#toLowerCase(Locale)
2584             * @since   1.1
2585             */
2586            public String toUpperCase(Locale locale) {
2587                if (locale == null) {
2588                    throw new NullPointerException();
2589                }
2590
2591                int firstLower;
2592
2593                /* Now check if there are any characters that need to be changed. */
2594                scan: {
2595                    for (firstLower = 0; firstLower < count;) {
2596                        int c = (int) value[offset + firstLower];
2597                        int srcCount;
2598                        if ((c >= Character.MIN_HIGH_SURROGATE)
2599                                && (c <= Character.MAX_HIGH_SURROGATE)) {
2600                            c = codePointAt(firstLower);
2601                            srcCount = Character.charCount(c);
2602                        } else {
2603                            srcCount = 1;
2604                        }
2605                        int upperCaseChar = Character.toUpperCaseEx(c);
2606                        if ((upperCaseChar == Character.ERROR)
2607                                || (c != upperCaseChar)) {
2608                            break scan;
2609                        }
2610                        firstLower += srcCount;
2611                    }
2612                    return this ;
2613                }
2614
2615                char[] result = new char[count]; /* may grow */
2616                int resultOffset = 0; /* result may grow, so i+resultOffset
2617                 * is the write location in result */
2618
2619                /* Just copy the first few upperCase characters. */
2620                System.arraycopy(value, offset, result, 0, firstLower);
2621
2622                String lang = locale.getLanguage();
2623                boolean localeDependent = (lang == "tr" || lang == "az" || lang == "lt");
2624                char[] upperCharArray;
2625                int upperChar;
2626                int srcChar;
2627                int srcCount;
2628                for (int i = firstLower; i < count; i += srcCount) {
2629                    srcChar = (int) value[offset + i];
2630                    if ((char) srcChar >= Character.MIN_HIGH_SURROGATE
2631                            && (char) srcChar <= Character.MAX_HIGH_SURROGATE) {
2632                        srcChar = codePointAt(i);
2633                        srcCount = Character.charCount(srcChar);
2634                    } else {
2635                        srcCount = 1;
2636                    }
2637                    if (localeDependent) {
2638                        upperChar = ConditionalSpecialCasing.toUpperCaseEx(
2639                                this , i, locale);
2640                    } else {
2641                        upperChar = Character.toUpperCaseEx(srcChar);
2642                    }
2643                    if ((upperChar == Character.ERROR)
2644                            || (upperChar >= Character.MIN_SUPPLEMENTARY_CODE_POINT)) {
2645                        if (upperChar == Character.ERROR) {
2646                            if (localeDependent) {
2647                                upperCharArray = ConditionalSpecialCasing
2648                                        .toUpperCaseCharArray(this , i, locale);
2649                            } else {
2650                                upperCharArray = Character
2651                                        .toUpperCaseCharArray(srcChar);
2652                            }
2653                        } else if (srcCount == 2) {
2654                            resultOffset += Character.toChars(upperChar,
2655                                    result, i + resultOffset)
2656                                    - srcCount;
2657                            continue;
2658                        } else {
2659                            upperCharArray = Character.toChars(upperChar);
2660                        }
2661
2662                        /* Grow result if needed */
2663                        int mapLen = upperCharArray.length;
2664                        if (mapLen > srcCount) {
2665                            char[] result2 = new char[result.length + mapLen
2666                                    - srcCount];
2667                            System.arraycopy(result, 0, result2, 0, i
2668                                    + resultOffset);
2669                            result = result2;
2670                        }
2671                        for (int x = 0; x < mapLen; ++x) {
2672                            result[i + resultOffset + x] = upperCharArray[x];
2673                        }
2674                        resultOffset += (mapLen - srcCount);
2675                    } else {
2676                        result[i + resultOffset] = (char) upperChar;
2677                    }
2678                }
2679                return new String(0, count + resultOffset, result);
2680            }
2681
2682            /**
2683             * Converts all of the characters in this <code>String</code> to upper
2684             * case using the rules of the default locale. This method is equivalent to
2685             * <code>toUpperCase(Locale.getDefault())</code>.
2686             * <p>
2687             * <b>Note:</b> This method is locale sensitive, and may produce unexpected
2688             * results if used for strings that are intended to be interpreted locale
2689             * independently.
2690             * Examples are programming language identifiers, protocol keys, and HTML
2691             * tags.
2692             * For instance, <code>"title".toUpperCase()</code> in a Turkish locale
2693             * returns <code>"T\u0130TLE"</code>, where '\u0130' is the LATIN CAPITAL
2694             * LETTER I WITH DOT ABOVE character.
2695             * To obtain correct results for locale insensitive strings, use
2696             * <code>toUpperCase(Locale.ENGLISH)</code>.
2697             * <p>
2698             * @return  the <code>String</code>, converted to uppercase.
2699             * @see     java.lang.String#toUpperCase(Locale)
2700             */
2701            public String toUpperCase() {
2702                return toUpperCase(Locale.getDefault());
2703            }
2704
2705            /**
2706             * Returns a copy of the string, with leading and trailing whitespace
2707             * omitted.
2708             * <p>
2709             * If this <code>String</code> object represents an empty character
2710             * sequence, or the first and last characters of character sequence
2711             * represented by this <code>String</code> object both have codes
2712             * greater than <code>'&#92;u0020'</code> (the space character), then a
2713             * reference to this <code>String</code> object is returned.
2714             * <p>
2715             * Otherwise, if there is no character with a code greater than
2716             * <code>'&#92;u0020'</code> in the string, then a new
2717             * <code>String</code> object representing an empty string is created
2718             * and returned.
2719             * <p>
2720             * Otherwise, let <i>k</i> be the index of the first character in the
2721             * string whose code is greater than <code>'&#92;u0020'</code>, and let
2722             * <i>m</i> be the index of the last character in the string whose code
2723             * is greater than <code>'&#92;u0020'</code>. A new <code>String</code>
2724             * object is created, representing the substring of this string that
2725             * begins with the character at index <i>k</i> and ends with the
2726             * character at index <i>m</i>-that is, the result of
2727             * <code>this.substring(<i>k</i>,&nbsp;<i>m</i>+1)</code>.
2728             * <p>
2729             * This method may be used to trim whitespace (as defined above) from
2730             * the beginning and end of a string.
2731             *
2732             * @return  A copy of this string with leading and trailing white
2733             *          space removed, or this string if it has no leading or
2734             *          trailing white space.
2735             */
2736            public String trim() {
2737                int len = count;
2738                int st = 0;
2739                int off = offset; /* avoid getfield opcode */
2740                char[] val = value; /* avoid getfield opcode */
2741
2742                while ((st < len) && (val[off + st] <= ' ')) {
2743                    st++;
2744                }
2745                while ((st < len) && (val[off + len - 1] <= ' ')) {
2746                    len--;
2747                }
2748                return ((st > 0) || (len < count)) ? substring(st, len) : this ;
2749            }
2750
2751            /**
2752             * This object (which is already a string!) is itself returned.
2753             *
2754             * @return  the string itself.
2755             */
2756            public String toString() {
2757                return this ;
2758            }
2759
2760            /**
2761             * Converts this string to a new character array.
2762             *
2763             * @return  a newly allocated character array whose length is the length
2764             *          of this string and whose contents are initialized to contain
2765             *          the character sequence represented by this string.
2766             */
2767            public char[] toCharArray() {
2768                char result[] = new char[count];
2769                getChars(0, count, result, 0);
2770                return result;
2771            }
2772
2773            /**
2774             * Returns a formatted string using the specified format string and
2775             * arguments.
2776             *
2777             * <p> The locale always used is the one returned by {@link
2778             * java.util.Locale#getDefault() Locale.getDefault()}.
2779             *
2780             * @param  format
2781             *         A <a href="../util/Formatter.html#syntax">format string</a>
2782             *
2783             * @param  args
2784             *         Arguments referenced by the format specifiers in the format
2785             *         string.  If there are more arguments than format specifiers, the
2786             *         extra arguments are ignored.  The number of arguments is
2787             *         variable and may be zero.  The maximum number of arguments is
2788             *         limited by the maximum dimension of a Java array as defined by
2789             *         the <a href="http://java.sun.com/docs/books/vmspec/">Java
2790             *         Virtual Machine Specification</a>.  The behaviour on a
2791             *         <tt>null</tt> argument depends on the <a
2792             *         href="../util/Formatter.html#syntax">conversion</a>.
2793             *
2794             * @throws  IllegalFormatException
2795             *          If a format string contains an illegal syntax, a format
2796             *          specifier that is incompatible with the given arguments,
2797             *          insufficient arguments given the format string, or other
2798             *          illegal conditions.  For specification of all possible
2799             *          formatting errors, see the <a
2800             *          href="../util/Formatter.html#detail">Details</a> section of the
2801             *          formatter class specification.
2802             *
2803             * @throws  NullPointerException
2804             *          If the <tt>format</tt> is <tt>null</tt>
2805             *
2806             * @return  A formatted string
2807             *
2808             * @see  java.util.Formatter
2809             * @since  1.5
2810             */
2811            public static String format(String format, Object... args) {
2812                return new Formatter().format(format, args).toString();
2813            }
2814
2815            /**
2816             * Returns a formatted string using the specified locale, format string,
2817             * and arguments.
2818             *
2819             * @param  l
2820             *         The {@linkplain java.util.Locale locale} to apply during
2821             *         formatting.  If <tt>l</tt> is <tt>null</tt> then no localization
2822             *         is applied.
2823             *
2824             * @param  format
2825             *         A <a href="../util/Formatter.html#syntax">format string</a>
2826             *
2827             * @param  args
2828             *         Arguments referenced by the format specifiers in the format
2829             *         string.  If there are more arguments than format specifiers, the
2830             *         extra arguments are ignored.  The number of arguments is
2831             *         variable and may be zero.  The maximum number of arguments is
2832             *         limited by the maximum dimension of a Java array as defined by
2833             *         the <a href="http://java.sun.com/docs/books/vmspec/">Java
2834             *         Virtual Machine Specification</a>.  The behaviour on a
2835             *         <tt>null</tt> argument depends on the <a
2836             *         href="../util/Formatter.html#syntax">conversion</a>.
2837             *
2838             * @throws  IllegalFormatException
2839             *          If a format string contains an illegal syntax, a format
2840             *          specifier that is incompatible with the given arguments,
2841             *          insufficient arguments given the format string, or other
2842             *          illegal conditions.  For specification of all possible
2843             *          formatting errors, see the <a
2844             *          href="../util/Formatter.html#detail">Details</a> section of the
2845             *          formatter class specification
2846             *
2847             * @throws  NullPointerException
2848             *          If the <tt>format</tt> is <tt>null</tt>
2849             *
2850             * @return  A formatted string
2851             *
2852             * @see  java.util.Formatter
2853             * @since  1.5
2854             */
2855            public static String format(Locale l, String format, Object... args) {
2856                return new Formatter(l).format(format, args).toString();
2857            }
2858
2859            /**
2860             * Returns the string representation of the <code>Object</code> argument.
2861             *
2862             * @param   obj   an <code>Object</code>.
2863             * @return  if the argument is <code>null</code>, then a string equal to
2864             *          <code>"null"</code>; otherwise, the value of
2865             *          <code>obj.toString()</code> is returned.
2866             * @see     java.lang.Object#toString()
2867             */
2868            public static String valueOf(Object obj) {
2869                return (obj == null) ? "null" : obj.toString();
2870            }
2871
2872            /**
2873             * Returns the string representation of the <code>char</code> array
2874             * argument. The contents of the character array are copied; subsequent
2875             * modification of the character array does not affect the newly
2876             * created string.
2877             *
2878             * @param   data   a <code>char</code> array.
2879             * @return  a newly allocated string representing the same sequence of
2880             *          characters contained in the character array argument.
2881             */
2882            public static String valueOf(char data[]) {
2883                return new String(data);
2884            }
2885
2886            /**
2887             * Returns the string representation of a specific subarray of the
2888             * <code>char</code> array argument.
2889             * <p>
2890             * The <code>offset</code> argument is the index of the first
2891             * character of the subarray. The <code>count</code> argument
2892             * specifies the length of the subarray. The contents of the subarray
2893             * are copied; subsequent modification of the character array does not
2894             * affect the newly created string.
2895             *
2896             * @param   data     the character array.
2897             * @param   offset   the initial offset into the value of the
2898             *                  <code>String</code>.
2899             * @param   count    the length of the value of the <code>String</code>.
2900             * @return  a string representing the sequence of characters contained
2901             *          in the subarray of the character array argument.
2902             * @exception IndexOutOfBoundsException if <code>offset</code> is
2903             *          negative, or <code>count</code> is negative, or
2904             *          <code>offset+count</code> is larger than
2905             *          <code>data.length</code>.
2906             */
2907            public static String valueOf(char data[], int offset, int count) {
2908                return new String(data, offset, count);
2909            }
2910
2911            /**
2912             * Returns a String that represents the character sequence in the
2913             * array specified.
2914             *
2915             * @param   data     the character array.
2916             * @param   offset   initial offset of the subarray.
2917             * @param   count    length of the subarray.
2918             * @return  a <code>String</code> that contains the characters of the
2919             *          specified subarray of the character array.
2920             */
2921            public static String copyValueOf(char data[], int offset, int count) {
2922                // All public String constructors now copy the data.
2923                return new String(data, offset, count);
2924            }
2925
2926            /**
2927             * Returns a String that represents the character sequence in the
2928             * array specified.
2929             *
2930             * @param   data   the character array.
2931             * @return  a <code>String</code> that contains the characters of the
2932             *          character array.
2933             */
2934            public static String copyValueOf(char data[]) {
2935                return copyValueOf(data, 0, data.length);
2936            }
2937
2938            /**
2939             * Returns the string representation of the <code>boolean</code> argument.
2940             *
2941             * @param   b   a <code>boolean</code>.
2942             * @return  if the argument is <code>true</code>, a string equal to
2943             *          <code>"true"</code> is returned; otherwise, a string equal to
2944             *          <code>"false"</code> is returned.
2945             */
2946            public static String valueOf(boolean b) {
2947                return b ? "true" : "false";
2948            }
2949
2950            /**
2951             * Returns the string representation of the <code>char</code>
2952             * argument.
2953             *
2954             * @param   c   a <code>char</code>.
2955             * @return  a string of length <code>1</code> containing
2956             *          as its single character the argument <code>c</code>.
2957             */
2958            public static String valueOf(char c) {
2959                char data[] = { c };
2960                return new String(0, 1, data);
2961            }
2962
2963            /**
2964             * Returns the string representation of the <code>int</code> argument.
2965             * <p>
2966             * The representation is exactly the one returned by the
2967             * <code>Integer.toString</code> method of one argument.
2968             *
2969             * @param   i   an <code>int</code>.
2970             * @return  a string representation of the <code>int</code> argument.
2971             * @see     java.lang.Integer#toString(int, int)
2972             */
2973            public static String valueOf(int i) {
2974                return Integer.toString(i, 10);
2975            }
2976
2977            /**
2978             * Returns the string representation of the <code>long</code> argument.
2979             * <p>
2980             * The representation is exactly the one returned by the
2981             * <code>Long.toString</code> method of one argument.
2982             *
2983             * @param   l   a <code>long</code>.
2984             * @return  a string representation of the <code>long</code> argument.
2985             * @see     java.lang.Long#toString(long)
2986             */
2987            public static String valueOf(long l) {
2988                return Long.toString(l, 10);
2989            }
2990
2991            /**
2992             * Returns the string representation of the <code>float</code> argument.
2993             * <p>
2994             * The representation is exactly the one returned by the
2995             * <code>Float.toString</code> method of one argument.
2996             *
2997             * @param   f   a <code>float</code>.
2998             * @return  a string representation of the <code>float</code> argument.
2999             * @see     java.lang.Float#toString(float)
3000             */
3001            public static String valueOf(float f) {
3002                return Float.toString(f);
3003            }
3004
3005            /**
3006             * Returns the string representation of the <code>double</code> argument.
3007             * <p>
3008             * The representation is exactly the one returned by the
3009             * <code>Double.toString</code> method of one argument.
3010             *
3011             * @param   d   a <code>double</code>.
3012             * @return  a  string representation of the <code>double</code> argument.
3013             * @see     java.lang.Double#toString(double)
3014             */
3015            public static String valueOf(double d) {
3016                return Double.toString(d);
3017            }
3018
3019            /**
3020             * Returns a canonical representation for the string object.
3021             * <p>
3022             * A pool of strings, initially empty, is maintained privately by the
3023             * class <code>String</code>.
3024             * <p>
3025             * When the intern method is invoked, if the pool already contains a
3026             * string equal to this <code>String</code> object as determined by
3027             * the {@link #equals(Object)} method, then the string from the pool is
3028             * returned. Otherwise, this <code>String</code> object is added to the
3029             * pool and a reference to this <code>String</code> object is returned.
3030             * <p>
3031             * It follows that for any two strings <code>s</code> and <code>t</code>,
3032             * <code>s.intern()&nbsp;==&nbsp;t.intern()</code> is <code>true</code>
3033             * if and only if <code>s.equals(t)</code> is <code>true</code>.
3034             * <p>
3035             * All literal strings and string-valued constant expressions are
3036             * interned. String literals are defined in &sect;3.10.5 of the
3037             * <a href="http://java.sun.com/docs/books/jls/html/">Java Language
3038             * Specification</a>
3039             *
3040             * @return  a string that has the same contents as this string, but is
3041             *          guaranteed to be from a pool of unique strings.
3042             */
3043            public native String intern();
3044
3045        }

