/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright The KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#ifndef JSON_SERIALIZERS_H_
#define JSON_SERIALIZERS_H_

#include <json_common.h>
#include <wx/gdicmn.h>
#include <wx/string.h>
#include <optional>

NLOHMANN_JSON_NAMESPACE_BEGIN
namespace detail
{
// silly hack to silence warnings about std::char_traits<wxUniChar> not being supported by the standard
// nolhmann json internally has its own char_traits templated to support some exotic char traits
template <>
struct char_traits<wxUniChar> : std::char_traits<char>
{
    using char_type = wxUniChar;
    using int_type = uint32_t;      // this is wxwidget's internal data type

    // Redefine to_int_type function
    static int_type to_int_type( char_type c ) noexcept { return static_cast<int_type>( c ); }

    static char_type to_char_type( int_type i ) noexcept { return static_cast<char_type>( i ); }

    static constexpr int_type eof() noexcept { return static_cast<int_type>( EOF ); }
};
} // namespace detail
NLOHMANN_JSON_NAMESPACE_END

namespace nlohmann
{
template <>
struct adl_serializer<wxString>
{
    static void from_json( const json& j, wxString& s )
    {
        s = wxString::FromUTF8( j.get<std::string>().c_str() );
    }

    static void to_json( json& j, const wxString& s ) { j = s.ToUTF8(); }
};

template <typename T>
struct adl_serializer<std::optional<T>>
{
    static void from_json( const json& j, std::optional<T>& opt )
    {
        if( j.is_null() )
        {
            opt = std::nullopt;
        }
        else
        {
            opt = j.template get<T>();
        }
    }

    static void to_json( json& j, const std::optional<T>& opt )
    {
        if( opt.has_value() )
        {
            j = *opt;
        }
        else
        {
            j = nullptr;
        }
    }
};

// Clang-specific stubs for wxPoint, wxSize, wxRect
#ifdef __clang__
template <>
    struct adl_serializer<wxPoint>
    {
        static void to_json(json& j, const wxPoint& p)
        {
            j["x"] = p.x;
            j["y"] = p.y;
        }

        static void from_json(const json& j, wxPoint& p)
        {
            j.at("x").get_to(p.x);
            j.at("y").get_to(p.y);
        }
    };

    template <>
    struct adl_serializer<wxSize>
    {
        static void to_json(json& j, const wxSize& s)
        {
            j["width"] = s.GetWidth();
            j["height"] = s.GetHeight();
        }

        static void from_json(const json& j, wxSize& s)
        {
            int w, h;
            j.at("width").get_to(w);
            j.at("height").get_to(h);
            s = wxSize(w, h);
        }
    };

    template <>
    struct adl_serializer<wxRect>
    {
        static void to_json(json& j, const wxRect& r)
        {
            j["x"] = r.x;
            j["y"] = r.y;
            j["width"] = r.width;
            j["height"] = r.height;
        }

        static void from_json(const json& j, wxRect& r)
        {
            int x, y, w, h;
            j.at("x").get_to(x);
            j.at("y").get_to(y);
            j.at("width").get_to(w);
            j.at("height").get_to(h);
            r = wxRect(x, y, w, h);
        }
    };
#endif // __clang__
} // namespace nlohmann


#endif // JSON_SERIALIZERS_H_
