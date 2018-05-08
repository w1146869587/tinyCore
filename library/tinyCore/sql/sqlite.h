#ifndef __TINY_CORE__SQL__SQLITE__H__
#define __TINY_CORE__SQL__SQLITE__H__


/**
 *
 *  作者: hm
 *
 *  说明: sqlite
 *
 */


#include <sqlite3.h>

#include <tinyCore/debug/trace.h>


namespace tinyCore
{
	namespace sql
	{
		class SQLiteQuery
		{
		public:
			SQLiteQuery() = default;

			SQLiteQuery(sqlite3 * db, sqlite3_stmt * stmt, const bool eof = true, const bool ownStmt = true) :
					_eof(eof),
					_ownStmt(ownStmt),
					_column(sqlite3_column_count(stmt)),
					_db(db),
					_stmt(stmt)
			{

			}

			SQLiteQuery(SQLiteQuery && rhs) noexcept : _eof(rhs._eof),
													   _ownStmt(rhs._ownStmt),
													   _column(rhs._column),
													   _db(rhs._db),
													   _stmt(rhs._stmt)
			{
				rhs._eof = true;
				rhs._ownStmt = false;
				rhs._column = 0;
				rhs._db = nullptr;
				rhs._stmt = nullptr;
			}

			SQLiteQuery(const SQLiteQuery & rhs) : _eof(rhs._eof),
												   _ownStmt(rhs._ownStmt),
												   _column(rhs._column),
												   _db(rhs._db),
												   _stmt(rhs._stmt)
			{
				const_cast<SQLiteQuery&>(rhs)._eof = true;
				const_cast<SQLiteQuery&>(rhs)._ownStmt = false;
				const_cast<SQLiteQuery&>(rhs)._column = 0;
				const_cast<SQLiteQuery&>(rhs)._db = nullptr;
				const_cast<SQLiteQuery&>(rhs)._stmt = nullptr;
			}

			virtual ~SQLiteQuery()
			{
				try
				{
					Finalize();
				}
				catch (...)
				{

				}
			}

			SQLiteQuery & operator=(SQLiteQuery && rhs) noexcept
			{
				try
				{
					Finalize();
				}
				catch (...)
				{

				}

				_eof = rhs._eof;
				_ownStmt = rhs._ownStmt;
				_column = rhs._column;
				_db = rhs._db;
				_stmt = rhs._stmt;

				rhs._eof = true;
				rhs._ownStmt = false;
				rhs._column = 0;
				rhs._db = nullptr;
				rhs._stmt = nullptr;

				return *this;
			}

			SQLiteQuery & operator=(const SQLiteQuery & rhs)
			{
				try
				{
					Finalize();
				}
				catch (...)
				{

				}

				_eof = rhs._eof;
				_ownStmt = rhs._ownStmt;
				_column = rhs._column;
				_db = rhs._db;
				_stmt = rhs._stmt;

				const_cast<SQLiteQuery&>(rhs)._eof = true;
				const_cast<SQLiteQuery&>(rhs)._ownStmt = false;
				const_cast<SQLiteQuery&>(rhs)._column = 0;
				const_cast<SQLiteQuery&>(rhs)._db = nullptr;
				const_cast<SQLiteQuery&>(rhs)._stmt = nullptr;

				return *this;
			}

			bool Eof() const
			{
				return _eof;
			}

			bool IsNullField(const char * field)
			{
				return IsNullField(FieldIndex(field));
			}

			bool IsNullField(const int32_t column)
			{
				return FieldType(column) == SQLITE_NULL;
			}

			bool IsNullDeclField(const char * field)
			{
				return IsNullDeclField(FieldIndex(field));
			}

			bool IsNullDeclField(const int32_t column)
			{
				return FieldDeclType(column) == nullptr;
			}

			void NextRow()
			{
				TINY_ASSERT(_stmt, "_stmt is nullptr")

				switch (sqlite3_step(_stmt))
				{
					case SQLITE_DONE:
					{
						_eof = true;

						break;
					}

					case SQLITE_ROW:
					{
						break;
					}

					default:
					{
						int ret = sqlite3_finalize(_stmt);

						_stmt = nullptr;

						TINY_THROW_EXCEPTION_IF(ret != SQLITE_OK, debug::SQLiteError, ret, sqlite3_errmsg(_db))
					}
				}
			}

			void Finalize()
			{
				if (_stmt && _ownStmt)
				{
					int ret = sqlite3_finalize(_stmt);

					_stmt = nullptr;

					TINY_THROW_EXCEPTION_IF(ret != SQLITE_OK, debug::SQLiteError, ret, sqlite3_errmsg(_db))
				}
			}

			int32_t Column() const
			{
				return _column;
			}

			int32_t FieldType(const int32_t column)
			{
				TINY_ASSERT(_stmt, "_stmt is nullptr")

				if (column < 0 || column >= _column)
				{
					TINY_THROW_EXCEPTION(debug::SQLiteError, SQLITE_ERROR, "Invalid field column")

					return -1;
				}

				return sqlite3_column_type(_stmt, column);
			}

			int32_t FieldIndex(const char * field)
			{
				TINY_ASSERT(field, "field is nullptr")

				if (field)
				{
					for (int32_t i = 0; i < _column; ++i)
					{
						auto name = FieldName(i);

						if (name && strcmp(field, name) == 0)
						{
							return i;
						}
					}
				}

				TINY_THROW_EXCEPTION(debug::SQLiteError, SQLITE_ERROR, "Invalid field")

				return -1;
			}

			int32_t GetInt32Field(const char * field, const int32_t nullValue = 0)
			{
				return GetInt32Field(FieldIndex(field), nullValue);
			}

			int32_t GetInt32Field(const int32_t column, const int32_t nullValue = 0)
			{
				TINY_ASSERT(_stmt, "_stmt is nullptr")

				if (IsNullField(column))
				{
					return nullValue;
				}
				else
				{
					return sqlite3_column_int(_stmt, column);
				}
			}

			sqlite_int64 GetInt64Field(const char * field, const sqlite_int64 nullValue = 0)
			{
				return GetInt64Field(FieldIndex(field), nullValue);
			}

			sqlite_int64 GetInt64Field(const int32_t column, const sqlite_int64 nullValue = 0)
			{
				TINY_ASSERT(_stmt, "_stmt is nullptr")

				if (IsNullField(column))
				{
					return nullValue;
				}
				else
				{
					return sqlite3_column_int64(_stmt, column);
				}
			}

			double GetDoubleField(const char * field, const double nullValue = 0.0)
			{
				return GetDoubleField(FieldIndex(field), nullValue);
			}

			double GetDoubleField(const int32_t column, const double nullValue = 0.0)
			{
				TINY_ASSERT(_stmt, "_stmt is nullptr")

				if (IsNullField(column))
				{
					return nullValue;
				}
				else
				{
					return sqlite3_column_double(_stmt, column);
				}
			}

			const char * FieldName(const int32_t column)
			{
				TINY_ASSERT(_stmt, "_stmt is nullptr")

				return sqlite3_column_name(_stmt, column);
			}

			const char * FieldDeclType(const int32_t column)
			{
				TINY_ASSERT(_stmt, "_stmt is nullptr")

				if (column < 0 || column >= _column)
				{
					TINY_THROW_EXCEPTION(debug::SQLiteError, SQLITE_ERROR, "Invalid field column")

					return nullptr;
				}

				return sqlite3_column_decltype(_stmt, column);
			}

			const char * FieldValue(const char * field)
			{
				return FieldValue(FieldIndex(field));
			}

			const char * FieldValue(const int32_t column)
			{
				TINY_ASSERT(_stmt, "_stmt is nullptr")

				if (column < 0 || column >= _column)
				{
					TINY_THROW_EXCEPTION(debug::SQLiteError, SQLITE_ERROR, "Invalid field column")

					return nullptr;
				}

				return (const char *)sqlite3_column_text(_stmt, column);
			}

			const char * GetTextField(const char * field, const char * nullValue = "")
			{
				return GetTextField(FieldIndex(field), nullValue);
			}

			const char * GetTextField(const int32_t column, const char * nullValue = "")
			{
				TINY_ASSERT(_stmt, "_stmt is nullptr")

				if (IsNullField(column))
				{
					return nullValue;
				}
				else
				{
					return (const char *)sqlite3_column_text(_stmt, column);
				}
			}

			const char * GetTextField(const char * field, int32_t * len, const char * nullValue = "")
			{
				return GetTextField(FieldIndex(field), len, nullValue);
			}

			const char * GetTextField(const int32_t column, int32_t * len, const char * nullValue = "")
			{
				TINY_ASSERT(_stmt, "_stmt is nullptr")

				if (IsNullField(column))
				{
					return nullValue;
				}
				else
				{
					if (len)
					{
						*len = sqlite3_column_bytes(_stmt, column);
					}

					return (const char *)sqlite3_column_text(_stmt, column);
				}
			}

			const void * GetBlobField(const char * field, const void * nullValue = "")
			{
				return GetBlobField(FieldIndex(field), nullValue);
			}
			
			const void * GetBlobField(const int32_t column, const void * nullValue = "")
			{
				TINY_ASSERT(_stmt, "_stmt is nullptr")

				if (IsNullField(column))
				{
					return nullValue;
				}
				else
				{
					return sqlite3_column_blob(_stmt, column);
				}
			}

			const void * GetBlobField(const char * field, int32_t * len, const void * nullValue = "")
			{
				return GetBlobField(FieldIndex(field), len, nullValue);
			}

			const void * GetBlobField(const int32_t column, int32_t * len, const void * nullValue = "")
			{
				TINY_ASSERT(_stmt, "_stmt is nullptr")

				if (IsNullField(column))
				{
					return nullValue;
				}
				else
				{
					if (len)
					{
						*len = sqlite3_column_bytes(_stmt, column);
					}

					return sqlite3_column_blob(_stmt, column);
				}
			}

		protected:
			bool _eof{ true };
			bool _ownStmt{ false };

			int32_t _column{ 0 };

			sqlite3 * _db{ nullptr };
			sqlite3_stmt * _stmt{ nullptr };
		};

		class SQLiteTable
		{
		public:
			SQLiteTable() = default;

			SQLiteTable(char ** result, const int32_t row, const int32_t column) : _row(row),
																				   _column(column),
																				   _currentRow(0),
																				   _result(result)
			{

			}

			SQLiteTable(SQLiteTable && rhs) noexcept : _row(rhs._row),
													   _column(rhs._column),
													   _currentRow(rhs._currentRow),
													   _result(rhs._result)
			{
				rhs._row = 0;
				rhs._column = 0;
				rhs._currentRow = 0;
				rhs._result = nullptr;
			}

			SQLiteTable(const SQLiteTable & rhs) : _row(rhs._row),
												   _column(rhs._column),
												   _currentRow(rhs._currentRow),
												   _result(rhs._result)
			{
				const_cast<SQLiteTable&>(rhs)._row = 0;
				const_cast<SQLiteTable&>(rhs)._column = 0;
				const_cast<SQLiteTable&>(rhs)._currentRow = 0;
				const_cast<SQLiteTable&>(rhs)._result = nullptr;
			}

			~SQLiteTable()
			{
				try
				{
					Finalize();
				}
				catch (...)
				{

				}
			}

			SQLiteTable & operator=(SQLiteTable && rhs) noexcept
			{
				try
				{
					Finalize();
				}
				catch (...)
				{

				}

				_row = rhs._row;
				_column = rhs._column;
				_currentRow = rhs._currentRow;
				_result = rhs._result;

				rhs._row = 0;
				rhs._column = 0;
				rhs._currentRow = 0;
				rhs._result = nullptr;

				return *this;
			}

			SQLiteTable & operator=(const SQLiteTable & rhs)
			{
				try
				{
					Finalize();
				}
				catch (...)
				{

				}

				_row = rhs._row;
				_column = rhs._column;
				_currentRow = rhs._currentRow;
				_result = rhs._result;

				const_cast<SQLiteTable&>(rhs)._row = 0;
				const_cast<SQLiteTable&>(rhs)._column = 0;
				const_cast<SQLiteTable&>(rhs)._currentRow = 0;
				const_cast<SQLiteTable&>(rhs)._result = nullptr;

				return *this;
			}

			bool IsNullField(const char * field)
			{
				return FieldValue(field) == nullptr;
			}

			bool IsNullField(const int32_t column)
			{
				return FieldValue(column) == nullptr;
			}

			void Finalize()
			{
				if (_result)
				{
					sqlite3_free_table(_result);

					_result = nullptr;
				}
			}

			void SetCurrentRow(const int32_t row)
			{
				_currentRow = row;
			}

			int32_t Row() const
			{
				return _row;
			}

			int32_t Column() const
			{
				return _column;
			}

			int32_t CurrentRow() const
			{
				return _currentRow;
			}

			int32_t GetIntField(const char * field, const int32_t nullValue = 0)
			{
				if (IsNullField(field))
				{
					return nullValue;
				}
				else
				{
					return TINY_STR_TO_DIGITAL(int32_t, FieldValue(field));
				}
			}

			int32_t GetIntField(const int32_t column, const int32_t nullValue = 0)
			{
				if (IsNullField(column))
				{
					return nullValue;
				}
				else
				{
					return TINY_STR_TO_DIGITAL(int32_t, FieldValue(column));
				}
			}

			double GetDoubleField(const char * field, const double nullValue = 0.0)
			{
				if (IsNullField(field))
				{
					return nullValue;
				}
				else
				{
					return TINY_STR_TO_DIGITAL(double, FieldValue(field));
				}
			}

			double GetDoubleField(const int32_t column, const double nullValue = 0.0)
			{
				if (IsNullField(column))
				{
					return nullValue;
				}
				else
				{
					return TINY_STR_TO_DIGITAL(double, FieldValue(column));
				}
			}

			const char * FieldName(const int32_t column)
			{
				TINY_ASSERT(_result, "_result is nullptr")

				if (column < 0 || column >= _column)
				{
					TINY_THROW_EXCEPTION(debug::SQLiteError, SQLITE_ERROR, "Invalid field column")

					return nullptr;
				}

				return _result[column];
			}

			const char * FieldValue(const char * field)
			{
				TINY_ASSERT(field, "field is nullptr")
				TINY_ASSERT(_result, "_result is nullptr")

				for (int i = 0; i < _column; ++i)
				{
					auto name = _result[i];

					if (name && strcmp(field, name) == 0)
					{
						return _result[((_currentRow + 1) * _column) + i];
					}
				}

				TINY_THROW_EXCEPTION(debug::SQLiteError, SQLITE_ERROR, "Invalid field")

				return nullptr;
			}

			const char * FieldValue(const int32_t column)
			{
				TINY_ASSERT(_result, "_result is nullptr")

				if (column < 0 || column >= _column)
				{
					TINY_THROW_EXCEPTION(debug::SQLiteError, SQLITE_ERROR, "Invalid field column")

					return nullptr;
				}

				return _result[((_currentRow + 1) * _column) + column];
			}

			const char * GetTextField(const char * field, const char * nullValue = "")
			{
				if (IsNullField(field))
				{
					return nullValue;
				}
				else
				{
					return FieldValue(field);
				}
			}

			const char * GetTextField(const int32_t column, const char * nullValue = "")
			{
				if (IsNullField(column))
				{
					return nullValue;
				}
				else
				{
					return FieldValue(column);
				}
			}

		protected:
			int32_t _row{ 0 };
			int32_t _column{ 0 };
			int32_t _currentRow{ 0 };

			char ** _result{ nullptr };
		};

		class SQLiteBinary
		{
		public:
			~SQLiteBinary()
			{
				Clear();
			}

			void Clear()
			{
				if (_buffer)
				{
					free(_buffer);

					_isEncode = false;

					_bufferLen = 0;
					_binaryLen = 0;
					_encodeLen = 0;
				}
			}

			void SetBinary(const unsigned char * buffer)
			{
				TINY_ASSERT(buffer, "buffer is nullptr")

				Clear();

				// Allow extra space for encoded binary as per comments in
				// SQLite encode.c See bottom of this file for implementation
				// of SQLite functions use 3 instead of 2 just to be sure ;-)
				_binaryLen = (int32_t)strlen((const char *)buffer);
				_bufferLen = 3 + (257 * _binaryLen) / 254;

				_buffer = (unsigned char *)malloc((size_t)_bufferLen);

				TINY_THROW_EXCEPTION_IF(_buffer == nullptr, debug::SQLiteError, SQLITE_ERROR, "Cannot allocate memory")

				memcpy(_buffer, buffer, (size_t)_bufferLen);

				_isEncode = false;
			}

			void SetEncode(const unsigned char * buffer)
			{
				TINY_ASSERT(buffer, "buffer is nullptr")

				Clear();

				_encodeLen = (int32_t)strlen((const char *)buffer);
				_bufferLen = _encodeLen + 1;

				_buffer = (unsigned char *)malloc((size_t)_bufferLen);

				TINY_THROW_EXCEPTION_IF(_buffer == nullptr, debug::SQLiteError, SQLITE_ERROR, "Cannot allocate memory")

				memcpy(_buffer, buffer, (size_t)_bufferLen);

				_isEncode = true;
			}

			int32_t BinaryLength()
			{
				(void)GetBinary();

				return _binaryLen;
			}

			const unsigned char * GetBinary()
			{
				if (_isEncode)
				{
					// in/out buffers can be the same
					_binaryLen = DecodeBinary(_buffer, _buffer);

					TINY_THROW_EXCEPTION_IF(_binaryLen == -1, debug::SQLiteError, SQLITE_ERROR, "Cannot decode binary")

					_isEncode = false;
				}

				return _buffer;
			}

			const unsigned char * GetEncode()
			{
				TINY_ASSERT(_buffer, "_buffer is nullptr")

				if (!_isEncode)
				{
					auto * temp = (unsigned char *)malloc((std::size_t)_binaryLen);

					TINY_THROW_EXCEPTION_IF(temp == nullptr, debug::SQLiteError, SQLITE_ERROR, "Cannot allocate memory")

					memset(temp, 0, (std::size_t)_binaryLen);

					memcpy(temp, _buffer, (std::size_t)_binaryLen);

					_encodeLen = EncodeBinary(temp, _binaryLen, _buffer);

					free(temp);

					_isEncode = true;
				}

				return _buffer;
			}

		protected:
			int32_t DecodeBinary(const unsigned char * in, unsigned char * out)
			{
				/**
				 *
				 * Decode the string "in" into binary data and write it into "out".
				 * This routine reverses the encoding created by sqlite3_encode_binary().
				 * The output will always be a few bytes less than the input.  The number
				 * of bytes of output is returned.  If the input is not a well-formed
				 * encoding, -1 is returned.
				 *
				 * The "in" and "out" parameters may point to the same buffer in order
				 * to decode a string in place.
				 *
				 */

				TINY_ASSERT(in, "in is nullptr")
				TINY_ASSERT(out, "out is nullptr")

				int32_t i = 0;
				int32_t c = 0;
				int32_t e = *(in++);

				while ((c = *(in++)) != 0)
				{
					if (c == 1)
					{
						c = *(in++);

						if (c == 1)
						{
							c = 0;
						}
						else if (c == 2)
						{
							c = 1;
						}
						else if (c == 3)
						{
							c = '\'';
						}
						else
						{
							return -1;
						}
					}

					out[i++] = (unsigned char)((c + e) & 0xff);
				}

				return i;
			}

			int32_t EncodeBinary(const unsigned char * in, const int32_t n, unsigned char * out)
			{
				/**
				 *
				 * How This Encoder Works
				 *
				 * The output is allowed to contain any character except 0x27 (') and
				 * 0x00.  This is accomplished by using an escape character to encode
				 * 0x27 and 0x00 as a two-byte sequence.  The escape character is always
				 * 0x01.  An 0x00 is encoded as the two byte sequence 0x01 0x01.  The
				 * 0x27 character is encoded as the two byte sequence 0x01 0x03.  Finally,
				 * the escape character itself is encoded as the two-character sequence
				 * 0x01 0x02.
				 *
				 * To summarize, the encoder works by using an escape sequences as follows:
				 *
				 *       0x00  ->  0x01 0x01
				 *       0x01  ->  0x01 0x02
				 *       0x27  ->  0x01 0x03
				 *
				 * If that were all the encoder did, it would work, but in certain cases
				 * it could double the size of the encoded string.  For example, to
				 * encode a string of 100 0x27 characters would require 100 instances of
				 * the 0x01 0x03 escape sequence resulting in a 200-character output.
				 * We would prefer to keep the size of the encoded string smaller than
				 * this.
				 *
				 * To minimize the encoding size, we first add a fixed offset value to each
				 * byte in the sequence.  The addition is modulo 256.  (That is to say, if
				 * the sum of the original character value and the offset exceeds 256, then
				 * the higher order bits are truncated.)  The offset is chosen to minimize
				 * the number of characters in the string that need to be escaped.  For
				 * example, in the case above where the string was composed of 100 0x27
				 * characters, the offset might be 0x01.  Each of the 0x27 characters would
				 * then be converted into an 0x28 character which would not need to be
				 * escaped at all and so the 100 character input string would be converted
				 * into just 100 characters of output.  Actually 101 characters of output -
				 * we have to record the offset used as the first byte in the sequence so
				 * that the string can be decoded.  Since the offset value is stored as
				 * part of the output string and the output string is not allowed to contain
				 * characters 0x00 or 0x27, the offset cannot be 0x00 or 0x27.
				 *
				 * Here, then, are the encoding steps:
				 *
				 *     (1)   Choose an offset value and make it the first character of
				 *           output.
				 *
				 *     (2)   Copy each input character into the output buffer, one by
				 *           one, adding the offset value as you copy.
				 *
				 *     (3)   If the value of an input character plus offset is 0x00, replace
				 *           that one character by the two-character sequence 0x01 0x01.
				 *           If the sum is 0x01, replace it with 0x01 0x02.  If the sum
				 *           is 0x27, replace it with 0x01 0x03.
				 *
				 *     (4)   Put a 0x00 terminator at the end of the output.
				 *
				 * Decoding is obvious:
				 *
				 *     (5)   Copy encoded characters except the first into the decode
				 *           buffer.  Set the first encoded character aside for use as
				 *           the offset in step 7 below.
				 *
				 *     (6)   Convert each 0x01 0x01 sequence into a single character 0x00.
				 *           Convert 0x01 0x02 into 0x01.  Convert 0x01 0x03 into 0x27.
				 *
				 *     (7)   Subtract the offset value that was the first character of
				 *           the encoded buffer from all characters in the output buffer.
				 *
				 * The only tricky part is step (1) - how to compute an offset value to
				 * minimize the size of the output buffer.  This is accomplished by testing
				 * all offset values and picking the one that results in the fewest number
				 * of escapes.  To do that, we first scan the entire input and count the
				 * number of occurances of each character value in the input.  Suppose
				 * the number of 0x00 characters is N(0), the number of occurances of 0x01
				 * is N(1), and so forth up to the number of occurances of 0xff is N(255).
				 * An offset of 0 is not allowed so we don't have to test it.  The number
				 * of escapes required for an offset of 1 is N(1)+N(2)+N(40).  The number
				 * of escapes required for an offset of 2 is N(2)+N(3)+N(41).  And so forth.
				 * In this way we find the offset that gives the minimum number of escapes,
				 * and thus minimizes the length of the output string.
				 *
				 */

				/**
				 *
				 * Encode a binary buffer "in" of size n bytes so that it contains
				 * no instances of characters '\'' or '\000'.  The output is
				 * null-terminated and can be used as a string value in an INSERT
				 * or UPDATE statement.  Use sqlite3_decode_binary() to convert the
				 * string back into its original binary.
				 *
				 * The result is written into a preallocated output buffer "out".
				 * "out" must be able to hold at least 2 +(257*n)/254 bytes.
				 * In other words, the output will be expanded by as much as 3
				 * bytes for every 254 bytes of input plus 2 bytes of fixed overhead.
				 * (This is approximately 2 + 1.0118*n or about a 1.2% size increase.)
				 *
				 * The return value is the number of characters in the encoded
				 * string, excluding the "\000" terminator.
				 *
				 */

				TINY_ASSERT(in, "in is nullptr")
				TINY_ASSERT(out, "out is nullptr")

				if (n <= 0)
				{
					out[0] = 'x';
					out[1] = 0;

					return 1;
				}

				int cnt[256];

				memset(cnt, 0, sizeof(cnt));

				for (int i = n - 1; i >= 0; --i)
				{
					cnt[in[i]]++;
				}

				int32_t m = n;
				int32_t j = 1;
				int32_t e = 0;

				for(int i = 1; i < 256; ++i)
				{
					if (i == '\'')
					{
						continue;
					}

					int sum = cnt[i] + cnt[(i + 1) & 0xff] + cnt[(i + '\'') & 0xff];

					if (sum < m)
					{
						m = sum;
						e = i;

						if (m == 0)
						{
							break;
						}
					}
				}

				out[0] = (unsigned char)e;

				for (int i = 0; i < n; ++i)
				{
					int c = (in[i] - e) & 0xff;

					if (c == 0)
					{
						out[j++] = 1;
						out[j++] = 1;
					}
					else if (c == 1)
					{
						out[j++] = 1;
						out[j++] = 2;
					}
					else if (c == '\'')
					{
						out[j++] = 1;
						out[j++] = 3;
					}
					else
					{
						out[j++] = (unsigned char)c;
					}
				}

				out[j] = 0;

				return j;
			}

		protected:
			bool _isEncode{ false };

			int32_t _bufferLen{ 0 };
			int32_t _binaryLen{ 0 };
			int32_t _encodeLen{ 0 };

			unsigned char * _buffer{ nullptr };
		};

		class SQLiteStatement
		{
		public:
			SQLiteStatement() = default;

			SQLiteStatement(sqlite3 * db, sqlite3_stmt * stmt) : _db(db),
																 _stmt(stmt)
			{

			}

			SQLiteStatement(SQLiteStatement && rhs) noexcept : _db(rhs._db),
															   _stmt(rhs._stmt)
			{
				rhs._db = nullptr;
				rhs._stmt = nullptr;
			}

			SQLiteStatement(const SQLiteStatement & rhs) : _db(rhs._db),
														   _stmt(rhs._stmt)
			{
				const_cast<SQLiteStatement&>(rhs)._db = nullptr;
				const_cast<SQLiteStatement&>(rhs)._stmt = nullptr;
			}

			~SQLiteStatement()
			{
				try
				{
					Finalize();
				}
				catch (...)
				{

				}
			}

			SQLiteStatement & operator=(SQLiteStatement && rhs) noexcept
			{
				try
				{
					Finalize();
				}
				catch (...)
				{

				}

				_db = rhs._db;
				_stmt = rhs._stmt;

				rhs._db = nullptr;
				rhs._stmt = nullptr;

				return *this;
			}

			SQLiteStatement & operator=(const SQLiteStatement & rhs)
			{
				try
				{
					Finalize();
				}
				catch (...)
				{

				}

				_db = rhs._db;
				_stmt = rhs._stmt;

				const_cast<SQLiteStatement&>(rhs)._db = nullptr;
				const_cast<SQLiteStatement&>(rhs)._stmt = nullptr;

				return *this;
			}

			void Reset()
			{
				if (_stmt)
				{
					int ret = sqlite3_reset(_stmt);

					TINY_THROW_EXCEPTION_IF(ret != SQLITE_OK, debug::SQLiteError, ret, sqlite3_errmsg(_db))
				}
			}

			void Finalize()
			{
				if (_stmt)
				{
					int ret = sqlite3_finalize(_stmt);

					_stmt = nullptr;

					TINY_THROW_EXCEPTION_IF(ret != SQLITE_OK, debug::SQLiteError, ret, sqlite3_errmsg(_db))
				}
			}

			void Bind(const int32_t param, const char * value)
			{
				TINY_ASSERT(_stmt, "_stmt is nullptr")
				TINY_ASSERT(value, "value is nullptr")

				int ret = sqlite3_bind_text(_stmt, param, value, -1, SQLITE_TRANSIENT);

				TINY_THROW_EXCEPTION_IF(ret != SQLITE_OK, debug::SQLiteError, ret, "Error bind text param")
			}

			void Bind(const int32_t param, const double value)
			{
				TINY_ASSERT(_stmt, "_stmt is nullptr")

				int ret = sqlite3_bind_double(_stmt, param, value);

				TINY_THROW_EXCEPTION_IF(ret != SQLITE_OK, debug::SQLiteError, ret, "Error bind double param")
			}

			void Bind(const int32_t param, const int32_t value)
			{
				TINY_ASSERT(_stmt, "_stmt is nullptr")

				int ret = sqlite3_bind_int(_stmt, param, value);

				TINY_THROW_EXCEPTION_IF(ret != SQLITE_OK, debug::SQLiteError, ret, "Error bind int param")
			}

			void Bind(const int32_t param, const unsigned char * value, const int32_t len)
			{
				TINY_ASSERT(_stmt, "_stmt is nullptr")
				TINY_ASSERT(value, "value is nullptr")

				int ret = sqlite3_bind_blob(_stmt, param, value, len, SQLITE_TRANSIENT);

				TINY_THROW_EXCEPTION_IF(ret != SQLITE_OK, debug::SQLiteError, ret, "Error bind blob param")
			}

			void Bind(const char * param, const char * value)
			{
				Bind(BindParameterIndex(param), value);
			}

			void Bind(const char * param, const double value)
			{
				Bind(BindParameterIndex(param), value);
			}

			void Bind(const char * param, const int32_t value)
			{
				Bind(BindParameterIndex(param), value);
			}

			void Bind(const char * param, const unsigned char * value, const int32_t len)
			{
				Bind(BindParameterIndex(param), value, len);
			}

			void BindNull(const char * param)
			{
				BindNull(BindParameterIndex(param));
			}

			void BindNull(const int32_t param)
			{
				TINY_ASSERT(_stmt, "_stmt is nullptr")

				int ret = sqlite3_bind_null(_stmt, param);

				TINY_THROW_EXCEPTION_IF(ret != SQLITE_OK, debug::SQLiteError, ret, "Error bind null param")
			}

			int32_t ExecDML()
			{
				TINY_ASSERT(_db, "_db is nullptr")
				TINY_ASSERT(_stmt, "_stmt is nullptr")

				if (sqlite3_step(_stmt))
				{
					int32_t rowsChanged = sqlite3_changes(_db);

					int32_t ret = sqlite3_reset(_stmt);

					TINY_THROW_EXCEPTION_IF(ret != SQLITE_OK, debug::SQLiteError, ret, sqlite3_errmsg(_db))

					return rowsChanged;
				}
				else
				{
					int32_t ret = sqlite3_reset(_stmt);

					TINY_THROW_EXCEPTION_IF(ret != SQLITE_OK, debug::SQLiteError, ret, sqlite3_errmsg(_db))
				}

				return -1;
			}

			int32_t BindParameterIndex(const char * param)
			{
				TINY_ASSERT(_stmt, "_stmt is nullptr")
				TINY_ASSERT(param, "param is nullptr")

				int32_t ret = sqlite3_bind_parameter_index(_stmt, param);

				// ?
				sqlite3_bind_parameter_count(_stmt);
				sqlite3_bind_parameter_name(_stmt, 1);
				sqlite3_bind_parameter_name(_stmt, 2);

				TINY_THROW_EXCEPTION_IF(ret != SQLITE_OK,
										debug::SQLiteError,
										ret,
										TINY_STR_FORMAT("Parameter '{}' is not valid for this statement", param))

				return ret;
			}

			SQLiteQuery ExecQuery()
			{
				TINY_ASSERT(_db, "_db is nullptr")
				TINY_ASSERT(_stmt, "_stmt is nullptr")

				int ret = sqlite3_step(_stmt);

				if (ret == SQLITE_DONE)
				{
					// no rows
					return SQLiteQuery(_db, _stmt, true);
				}
				else if (ret == SQLITE_ROW)
				{
					// at least 1 row
					return SQLiteQuery(_db, _stmt, false);
				}
				else
				{
					ret = sqlite3_reset(_stmt);

					TINY_THROW_EXCEPTION(debug::SQLiteError, ret, sqlite3_errmsg(_db));
				}

				return SQLiteQuery(_db, _stmt, true, false);
			}

		protected:
			sqlite3 * _db{ nullptr };
			sqlite3_stmt * _stmt{ nullptr };
		};

		class SQLiteDataBase
		{
		public:
			SQLiteDataBase() = default;

			SQLiteDataBase(sqlite3 * db, int32_t busyTimeout) : _db(db),
																_busyTimeout(busyTimeout)
			{

			}

			SQLiteDataBase(SQLiteDataBase && rhs) noexcept : _db(rhs._db),
															 _busyTimeout(rhs._busyTimeout)
			{
				rhs._db = nullptr;
				rhs._busyTimeout = TINY_MINUTE;
			}

			SQLiteDataBase(const SQLiteDataBase & rhs) : _db(rhs._db),
														 _busyTimeout(rhs._busyTimeout)
			{
				const_cast<SQLiteDataBase&>(rhs)._db = nullptr;
				const_cast<SQLiteDataBase&>(rhs)._busyTimeout = TINY_MINUTE;
			}

			~SQLiteDataBase()
			{
				try
				{
					Close();
				}
				catch (...)
				{

				}
			}

			SQLiteDataBase & operator=(SQLiteDataBase && rhs) noexcept
			{
				_db = rhs._db;
				_busyTimeout = rhs._busyTimeout;

				rhs._db = nullptr;
				rhs._busyTimeout = TINY_MINUTE;

				return *this;
			}

			SQLiteDataBase & operator=(const SQLiteDataBase & rhs)
			{
				_db = rhs._db;
				_busyTimeout = rhs._busyTimeout;

				const_cast<SQLiteDataBase&>(rhs)._db = nullptr;
				const_cast<SQLiteDataBase&>(rhs)._busyTimeout = 0;

				return *this;
			}

			void Close()
			{
				if (_db)
				{
					int ret = sqlite3_close(_db);

					_db = nullptr;

					TINY_THROW_EXCEPTION_IF(ret != SQLITE_OK, debug::SQLiteError, ret, "Unable to close database")
				}
			}

			void Interrupt()
			{
				TINY_ASSERT(_db, "_db is nullptr")

				sqlite3_interrupt(_db);
			}

			void Open(const char * name)
			{
				TINY_ASSERT(name, "name is nullptr")

				int32_t ret = sqlite3_open(name, &_db);

				TINY_THROW_EXCEPTION_IF(ret != SQLITE_OK, debug::SQLiteError, ret, sqlite3_errmsg(_db))

				SetBusyTimeout(_busyTimeout);
			}

			void SetBusyTimeout(const int32_t millisecond)
			{
				TINY_ASSERT(_db, "_db is nullptr")

				_busyTimeout = millisecond;

				int32_t ret = sqlite3_busy_timeout(_db, millisecond);

				TINY_THROW_EXCEPTION_IF(ret != SQLITE_OK, debug::SQLiteError, ret, sqlite3_errmsg(_db))
			}

			bool IsAutoCommit()
			{
				TINY_ASSERT(_db, "_db is nullptr")

				return sqlite3_get_autocommit(_db) != 0;
			}

			bool IsExistTable(const char * table)
			{
				TINY_ASSERT(table, "table is nullptr")

				auto sql = TINY_STR_FORMAT("select count(*) from sqlite_master where type='table' and name='{}'", table);

				return (ExecScalar(sql.c_str()) > 0);
			}

			int32_t ExecDML(const char * sql)
			{
				TINY_ASSERT(_db, "_db is nullptr")
				TINY_ASSERT(sql, "table is nullptr")

				char * errorMsg = nullptr;

				int32_t ret = sqlite3_exec(_db, sql, nullptr, nullptr, &errorMsg);

				if (ret == SQLITE_OK)
				{
					return sqlite3_changes(_db);
				}
				else
				{
					std::string str = TINY_STR_TO_STRING(errorMsg);

					sqlite3_free(errorMsg);

					TINY_THROW_EXCEPTION(debug::SQLiteError, ret, str, sql)
				}

				return -1;
			}

			int32_t ExecScalar(const char * sql, const int32_t nullValue = 0)
			{
				SQLiteQuery query = ExecQuery(sql);

				TINY_THROW_EXCEPTION_IF(query.Eof() || query.Column() <= 0, debug::SQLiteError, SQLITE_ERROR, "Invalid scalar")

				return query.GetInt32Field(0, nullValue);
			}

			int32_t DropTable(const char * table)
			{
				TINY_ASSERT(table, "table is nullptr")

				return ExecDML(TINY_STR_CFORMAT("DROP TABLE '{}'", table));
			}

			int32_t DeleteTable(const char * table)
			{
				TINY_ASSERT(table, "table is nullptr")

				return ExecDML(TINY_STR_CFORMAT("DELETE FROM '{}'", table));
			}

			sqlite_int64 LastInsertRowID()
			{
				TINY_ASSERT(_db, "_db is nullptr")

				return sqlite3_last_insert_rowid(_db);
			}

			SQLiteTable GetTable(const char * sql)
			{
				TINY_ASSERT(_db, "_db is nullptr")
				TINY_ASSERT(sql, "sql is nullptr")

				int row = 0;
				int column = 0;

				char ** results = nullptr;
				char *  errorMsg = nullptr;

				int ret = sqlite3_get_table(_db, sql, &results, &row, &column, &errorMsg);

				if (ret == SQLITE_OK)
				{
					return SQLiteTable(results, row, column);
				}
				else
				{
					std::string str = TINY_STR_TO_STRING(errorMsg);

					sqlite3_free(errorMsg);

					TINY_THROW_EXCEPTION(debug::SQLiteError, ret, str, sql)
				}

				return SQLiteTable(nullptr, 0, 0);
			}

			SQLiteQuery ExecQuery(const char * sql)
			{
				TINY_ASSERT(_db, "_db is nullptr")
				TINY_ASSERT(sql, "sql is nullptr")

				sqlite3_stmt * stmt = Compile(sql);

				int32_t ret = sqlite3_step(stmt);

				if (ret == SQLITE_DONE)
				{
					// no rows
					return SQLiteQuery(_db, stmt, true);
				}
				else if (ret == SQLITE_ROW)
				{
					// at least 1 row
					return SQLiteQuery(_db, stmt, false);
				}
				else
				{
					ret = sqlite3_finalize(stmt);

					TINY_THROW_EXCEPTION(debug::SQLiteError, ret, sqlite3_errmsg(_db), sql)
				}

				return SQLiteQuery(_db, stmt, true, false);
			}

			SQLiteStatement CompileStatement(const char * sql)
			{
				TINY_ASSERT(_db, "_db is nullptr")

				sqlite3_stmt * stmt = Compile(sql);

				return SQLiteStatement(_db, stmt);
			}

			static const char * SQLiteVersion()
			{
				return SQLITE_VERSION;
			}

			static const char * SQLiteLibraryVersion()
			{
				return sqlite3_libversion();
			}

			static int32_t SQLiteLibraryVersionNumber()
			{
				return sqlite3_libversion_number();
			}

		protected:
			sqlite3_stmt * Compile(const char * sql)
			{
				TINY_ASSERT(_db, "_db is nullptr")
				TINY_ASSERT(sql, "sql is nullptr")

				const char * tail = nullptr;

				sqlite3_stmt * stmt = nullptr;

				int32_t ret = sqlite3_prepare_v2(_db, sql, -1, &stmt, &tail);

				TINY_THROW_EXCEPTION_IF(ret != SQLITE_OK, debug::SQLiteError, ret, sqlite3_errmsg(_db), sql)

				return stmt;
			}

		protected:
			sqlite3 * _db{ nullptr };

			int32_t _busyTimeout{ TINY_MINUTE };
		};
	}
}


#endif // __TINY_CORE__SQL__SQLITE__H__
