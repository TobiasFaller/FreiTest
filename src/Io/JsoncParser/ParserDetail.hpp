// ----------------------------------------------------------------------------
// Copyright (C) 2002-2006 Marcin Kalicinski
// Copyright (C) 2015 Sebastian Redl
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see www.boost.org
// ----------------------------------------------------------------------------

#ifndef BOOST_PROPERTY_TREE_DETAIL_JSONC_PARSER_PARSER_HPP
#define BOOST_PROPERTY_TREE_DETAIL_JSONC_PARSER_PARSER_HPP

#include <boost/property_tree/json_parser/error.hpp>
#include <boost/core/ref.hpp>
#include <boost/bind/bind.hpp>
#include <boost/format.hpp>
#include <boost/bind.hpp>

#include <iterator>
#include <sstream>
#include <string>

namespace boost { namespace property_tree {
    namespace jsonc_parser { namespace detail
{

    using json_parser_error = json_parser::json_parser_error;

    template <typename Encoding, typename Iterator, typename Sentinel>
    class source
    {
    public:
        typedef typename std::iterator_traits<Iterator>::value_type
            code_unit;
        typedef bool (Encoding::*encoding_predicate)(code_unit c) const;

        explicit source(Encoding& encoding) : encoding(encoding) {}

        template <typename Range>
        void set_input(const std::string& filename, const Range& r)
        {
            this->filename = filename;
            cur = r.begin();
            end = r.end();
            // Note that there is no backtracking, so if e.g. a UTF-8 file
            // starts with something that initially looks like a BOM but isn't,
            // there's trouble.
            // However, no valid JSON file can start with a UTF-8 EF byte.
            encoding.skip_introduction(cur, end);
            line = 1;
            offset = 0;
        }

        bool done() const { return cur == end; }

        void parse_error(const char* msg) {
            BOOST_PROPERTY_TREE_THROW(
                json_parser_error(msg, filename, line));
        }

        void next() {
            if (encoding.is_nl(*cur)) {
                ++line;
                offset = 0;
            } else {
                ++offset;
            }
            ++cur;
        }

        template <typename Action>
        bool have(encoding_predicate p, Action& a) {
            bool found = cur != end && (encoding.*p)(*cur);
            if (found) {
                a(*cur);
                next();
            }
            return found;
        }

        bool have(encoding_predicate p) {
            DoNothing n;
            return have(p, n);
        }

        template <typename Action>
        void expect(encoding_predicate p, const char* msg, Action& a) {
            if (!have(p, a)) {
                parse_error(msg);
            }
        }

        void expect(encoding_predicate p, const char* msg) {
            DoNothing n;
            expect(p, msg, n);
        }

        code_unit need_cur(const char* msg) {
            if (cur == end) {
                parse_error(msg);
            }
            return *cur;
        }

        Iterator& raw_cur() { return cur; }
        Sentinel raw_end() { return end; }

    private:
        struct DoNothing {
            void operator ()(code_unit) const {}
        };

        Encoding& encoding;
        Iterator cur;
        Sentinel end;
        std::string filename;
        int line;
        int offset;
    };

    template <typename Callbacks, typename Encoding, typename Iterator,
        typename = typename std::iterator_traits<Iterator>
            ::iterator_category>
    class number_callback_adapter
    {
    public:
        number_callback_adapter(Callbacks& callbacks, Encoding& encoding,
                                Iterator& cur)
            : callbacks(callbacks), encoding(encoding), first(cur), cur(cur)
        {}

        void operator ()(typename Encoding::external_char) {}

        void finish() const {
            callbacks.on_number(encoding.to_internal(first, cur));
        }

    private:
        number_callback_adapter(const number_callback_adapter&);

        Callbacks& callbacks;
        Encoding& encoding;
        Iterator first;
        Iterator& cur;
    };

    template <typename Callbacks, typename Encoding, typename Iterator>
    class number_callback_adapter<Callbacks, Encoding, Iterator,
                                  std::input_iterator_tag>
    {
    public:
        number_callback_adapter(Callbacks& callbacks, Encoding& encoding,
                                Iterator&)
            : callbacks(callbacks), encoding(encoding), first(true)
        {}

        void operator ()(typename Encoding::external_char c) {
            if (first) {
                callbacks.on_begin_number();
                first = false;
            }
            callbacks.on_digit(encoding.to_internal_trivial(c));
        }

        void finish() const {
            callbacks.on_end_number();
        }
    private:
        number_callback_adapter(const number_callback_adapter&);

        Callbacks& callbacks;
        Encoding& encoding;
        bool first;
    };

    template <typename Callbacks, typename Encoding, typename Iterator,
        typename = typename std::iterator_traits<Iterator>
            ::iterator_category>
    class string_callback_adapter
    {
    public:
        string_callback_adapter(Callbacks& callbacks, Encoding& encoding,
                                Iterator& cur)
            : callbacks(callbacks), encoding(encoding), cur(cur),
              run_begin(cur)
        {}

        void start_run() {
            run_begin = cur;
        }

        void finish_run() {
            callbacks.on_code_units(encoding.to_internal(run_begin, cur));
        }

        template <typename Sentinel, typename EncodingErrorFn>
        void process_codepoint(Sentinel end, EncodingErrorFn error_fn) {
            encoding.skip_codepoint(cur, end, error_fn);
        }

    private:
        string_callback_adapter(const string_callback_adapter&);

        Callbacks& callbacks;
        Encoding& encoding;
        Iterator& cur;
        Iterator run_begin;
    };

    template <typename Callbacks, typename Encoding, typename Iterator>
    class string_callback_adapter<Callbacks, Encoding, Iterator,
                                  std::input_iterator_tag>
    {
    public:
        string_callback_adapter(Callbacks& callbacks, Encoding& encoding,
                                Iterator& cur)
            : callbacks(callbacks), encoding(encoding), cur(cur)
        {}

        void start_run() {}

        void finish_run() {}

        template <typename Sentinel, typename EncodingErrorFn>
        void process_codepoint(Sentinel end, EncodingErrorFn error_fn) {
            encoding.transcode_codepoint(cur, end,
                boost::bind(&Callbacks::on_code_unit,
                            boost::ref(callbacks), boost::placeholders::_1),
                error_fn);
        }

    private:
        string_callback_adapter(const string_callback_adapter&);

        Callbacks& callbacks;
        Encoding& encoding;
        Iterator& cur;
    };

    template <typename Callbacks, typename Encoding, typename Iterator,
              typename Sentinel>
    class parser
    {
        typedef detail::number_callback_adapter<Callbacks, Encoding, Iterator>
            number_adapter;
        typedef detail::string_callback_adapter<Callbacks, Encoding, Iterator>
            string_adapter;
        typedef detail::source<Encoding, Iterator, Sentinel> source;
        typedef typename source::code_unit code_unit;

    public:
        parser(Callbacks& callbacks, Encoding& encoding)
            : callbacks(callbacks), encoding(encoding), src(encoding)
        {}

        template <typename Range>
        void set_input(const std::string& filename, const Range& r) {
            src.set_input(filename, r);
        }

        void finish() {
            skip_ws();
            if (!src.done()) {
                parse_error("garbage after data");
            }
        }

        void parse_value() {
            if (parse_object()) return;
            if (parse_array()) return;
            if (parse_string()) return;
            if (parse_boolean()) return;
            if (parse_null()) return;
            if (parse_number()) return;
            parse_error("expected value");
        }

        bool parse_null() {
            skip_ws();
            if (!have(&Encoding::is_n)) {
                return false;
            }
            expect(&Encoding::is_u, "expected 'null'");
            expect(&Encoding::is_l, "expected 'null'");
            expect(&Encoding::is_l, "expected 'null'");
            callbacks.on_null();
            return true;
        }

        bool parse_boolean() {
            skip_ws();
            if (have(&Encoding::is_t)) {
                expect(&Encoding::is_r, "expected 'true'");
                expect(&Encoding::is_u, "expected 'true'");
                expect(&Encoding::is_e, "expected 'true'");
                callbacks.on_boolean(true);
                return true;
            }
            if (have(&Encoding::is_f)) {
                expect(&Encoding::is_a, "expected 'false'");
                expect(&Encoding::is_l, "expected 'false'");
                expect(&Encoding::is_s, "expected 'false'");
                expect(&Encoding::is_e, "expected 'false'");
                callbacks.on_boolean(false);
                return true;
            }
            return false;
        }

        bool parse_number() {
            skip_ws();

            number_adapter adapter(callbacks, encoding, src.raw_cur());
            bool started = false;
            if (have(&Encoding::is_minus, adapter)) {
                started = true;
            }
            if (!have(&Encoding::is_0, adapter) && !parse_int_part(adapter)) {
                if (started) {
                    parse_error("expected digits after -");
                }
                return false;
            }
            parse_frac_part(adapter);
            parse_exp_part(adapter);
            adapter.finish();
            return true;
        }

        bool parse_string() {
            skip_ws();

            if (!have(&Encoding::is_quote)) {
                return false;
            }

            callbacks.on_begin_string();
            string_adapter adapter(callbacks, encoding, src.raw_cur());
            while (!encoding.is_quote(need_cur("unterminated string"))) {
                if (encoding.is_backslash(*src.raw_cur())) {
                    adapter.finish_run();
                    next();
                    parse_escape();
                    adapter.start_run();
                } else {
                    adapter.process_codepoint(src.raw_end(),
                        boost::bind(&parser::parse_error,
                                    this, "invalid code sequence"));
                }
            }
            adapter.finish_run();
            callbacks.on_end_string();
            next();
            return true;
        }

        bool parse_array() {
            skip_ws();

            if (!have(&Encoding::is_open_bracket)) {
                return false;
            }

            callbacks.on_begin_array();
            do {
                // Trailing comma is allowed in JSONC
                skip_ws();
                if (have(&Encoding::is_close_bracket)) {
                    callbacks.on_end_array();
                    return true;
                }

                parse_value();
                skip_ws();
            } while (have(&Encoding::is_comma));
            expect(&Encoding::is_close_bracket, "expected ']' or ','");
            callbacks.on_end_array();
            return true;
        }

        bool parse_object() {
            skip_ws();

            if (!have(&Encoding::is_open_brace)) {
                return false;
            }

            callbacks.on_begin_object();
            do {
                // Trailing comma is allowed in JSONC
                skip_ws();
                if (have(&Encoding::is_close_brace)) {
                    callbacks.on_end_object();
                    return true;
                }

                if (!parse_string()) {
                    parse_error("expected key string");
                }
                skip_ws();
                expect(&Encoding::is_colon, "expected ':'");
                parse_value();
                skip_ws();
            } while (have(&Encoding::is_comma));
            expect(&Encoding::is_close_brace, "expected '}' or ','");
            callbacks.on_end_object();
            return true;
        }

    private:
        typedef typename source::encoding_predicate encoding_predicate;

        void parse_error(const char* msg) { src.parse_error(msg); }
        void next() { src.next(); }
        template <typename Action>
        bool have(encoding_predicate p, Action& a) { return src.have(p, a); }
        bool have(encoding_predicate p) { return src.have(p); }
        template <typename Action>
        void expect(encoding_predicate p, const char* msg, Action& a) {
            src.expect(p, msg, a);
        }
        void expect(encoding_predicate p, const char* msg) {
            src.expect(p, msg);
        }
        code_unit need_cur(const char* msg) { return src.need_cur(msg); }
        bool has_more() { return src.raw_cur() != src.raw_end(); }

        void skip_ws() {
            while (have(&Encoding::is_ws) || skip_comment()) { }
        }

        bool skip_comment() {
            // After a slash a comment follows
            if (have(&Encoding::is_slash)) {
                // Single-line comment has the form "//CONTENT\n"
                if (have(&Encoding::is_slash)) {
                    while (has_more() && !have(&Encoding::is_nl)) {
                        next();
                    }

                    return true;
                }

                // Multi-line comment has the form "/*CONTENT*/"
                if (have(&Encoding::is_star)) {
                    need_cur("unterminated multi-line comment");

                    while (!have(&Encoding::is_star) || !have(&Encoding::is_slash))
                    {
                        next();
                        need_cur("unterminated multi-line comment");
                    }

                    return true;
                }

                parse_error("expected '/' or '*' (start of a comment)");
            }

            return false;
        }

        bool parse_int_part(number_adapter& action) {
            if (!have(&Encoding::is_digit0, action)) {
                return false;
            }
            parse_digits(action);
            return true;
        }

        void parse_frac_part(number_adapter& action) {
            if (!have(&Encoding::is_dot, action)) {
                return;
            }
            expect(&Encoding::is_digit, "need at least one digit after '.'",
                   action);
            parse_digits(action);
        }

        void parse_exp_part(number_adapter& action) {
            if (!have(&Encoding::is_eE, action)) {
                return;
            }
            have(&Encoding::is_plusminus, action);
            expect(&Encoding::is_digit, "need at least one digit in exponent",
                   action);
            parse_digits(action);
        }

        void parse_digits(number_adapter& action) {
            while (have(&Encoding::is_digit, action)) {
            }
        }

        void parse_escape() {
            if (have(&Encoding::is_quote)) {
                feed(0x22);
            } else if (have(&Encoding::is_backslash)) {
                feed(0x5c);
            } else if (have(&Encoding::is_slash)) {
                feed(0x2f);
            } else if (have(&Encoding::is_b)) {
                feed(0x08); // backspace
            } else if (have(&Encoding::is_f)) {
                feed(0x0c); // formfeed
            } else if (have(&Encoding::is_n)) {
                feed(0x0a); // line feed
            } else if (have(&Encoding::is_r)) {
                feed(0x0d); // carriage return
            } else if (have(&Encoding::is_t)) {
                feed(0x09); // horizontal tab
            } else if (have(&Encoding::is_u)) {
                parse_codepoint_ref();
            } else {
                parse_error("invalid escape sequence");
            }
        }

        unsigned parse_hex_quad() {
            unsigned codepoint = 0;
            for (int i = 0; i < 4; ++i) {
                int value = encoding.decode_hexdigit(
                    need_cur("invalid escape sequence"));
                if (value < 0) {
                    parse_error("invalid escape sequence");
                }
                codepoint *= 16;
                codepoint += value;
                next();
            }
            return codepoint;
        }

        static bool is_surrogate_high(unsigned codepoint) {
            return (codepoint & 0xfc00) == 0xd800;
        }
        static bool is_surrogate_low(unsigned codepoint) {
            return (codepoint & 0xfc00) == 0xdc00;
        }
        static unsigned combine_surrogates(unsigned high, unsigned low) {
            return 0x010000 + (((high & 0x3ff) << 10) | (low & 0x3ff));
        }

        void parse_codepoint_ref() {
            unsigned codepoint = parse_hex_quad();
            if (is_surrogate_low(codepoint)) {
                parse_error("invalid codepoint, stray low surrogate");
            }
            if (is_surrogate_high(codepoint)) {
                expect(&Encoding::is_backslash,
                    "invalid codepoint, stray high surrogate");
                expect(&Encoding::is_u,
                    "expected codepoint reference after high surrogate");
                int low = parse_hex_quad();
                if (!is_surrogate_low(low)) {
                    parse_error("expected low surrogate after high surrogate");
                }
                codepoint = combine_surrogates(codepoint, low);
            }
            feed(codepoint);
        }

        void feed(unsigned codepoint) {
            encoding.feed_codepoint(codepoint,
                boost::bind(&Callbacks::on_code_unit,
                    boost::ref(callbacks), boost::placeholders::_1));
        }

        Callbacks& callbacks;
        Encoding& encoding;
        source src;
    };

}}}}

#endif
