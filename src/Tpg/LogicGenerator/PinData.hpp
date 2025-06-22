#pragma once

#include <cstdint>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "Tpg/Container/LogicContainer.hpp"

namespace FreiTest
{
namespace Tpg
{

// ----------------------------------------------------------------------------
// Tags and encodings
// ----------------------------------------------------------------------------

struct NodeTag {
	// Virtual function is required for correct dynamic typeid identification.
	virtual std::string GetName() const = 0;
};

template<typename PinData, typename Tag> struct has_pin_data_tag { using type = std::is_base_of<Tag, PinData>; };
template<typename PinData, typename Tag> using has_pin_data_tag_t = typename has_pin_data_tag<PinData, Tag>::type;
template<typename PinData, typename Tag> constexpr bool has_pin_data_tag_v = has_pin_data_tag_t<PinData, Tag>::value;

template<typename PinData, typename Tag> struct has_pin_data_encoding { using type = std::false_type; };
template<typename PinData, typename Tag> using has_pin_data_encoding_t = typename has_pin_data_encoding<PinData, Tag>::type;
template<typename PinData, typename Tag> constexpr bool has_pin_data_encoding_v = has_pin_data_encoding_t<PinData, Tag>::value;

template<typename PinData, typename Tag, typename enable = std::enable_if_t<has_pin_data_tag_v<PinData, Tag>>> struct get_pin_data_container { };
template<typename PinData, typename Tag, typename enable = std::enable_if_t<has_pin_data_tag_v<PinData, Tag>>> using get_pin_data_container_t = typename get_pin_data_container<PinData, Tag>::type;

template<typename PinData, typename Tag, typename enable = std::enable_if_t<has_pin_data_encoding_v<PinData, Tag>>> struct get_pin_data_encoding { };
template<typename PinData, typename Tag, typename enable = std::enable_if_t<has_pin_data_encoding_v<PinData, Tag>>> using get_pin_data_encoding_t = typename get_pin_data_encoding<PinData, Tag>::type;

template<typename Tag> struct get_tag_name { };
template<typename Tag> constexpr auto get_tag_name_v = get_tag_name<Tag>::value;

#define DEFINE_PINDATA_TAG(NAME, FIELD) \
	struct NAME ## Tag: public NodeTag { std::string GetName() const override { return #NAME; } }; \
	template<typename PinData> struct get_pin_data_container<PinData, NAME ## Tag> { using type = typename PinData::LogicContainer ## NAME ## Type; }; \
	template<typename PinData> using          has_ ## FIELD ## _tag = has_pin_data_tag_t<PinData, NAME ## Tag>; \
	template<typename PinData> constexpr bool has_ ## FIELD ## _tag_v = has_pin_data_tag_t<PinData, NAME ## Tag>::value; \
	template<> struct get_tag_name<NAME ## Tag> { static constexpr const char* value = #NAME; };

#define DEFINE_PINDATA_ENCODING(NAME, FIELD) DEFINE_PINDATA_TAG(NAME, FIELD) \
    template<typename Container> struct NAME ## Encoding { Container FIELD; }; \
	template<typename PinData> struct has_pin_data_encoding<PinData, NAME ## Tag> { \
        template<typename Container> \
        static constexpr std::true_type test(const NAME ## Encoding<Container> *); \
        static constexpr std::false_type test(...); \
        using type = decltype(test(std::declval<PinData*>())); \
    }; \
	template<typename PinData> struct get_pin_data_encoding<PinData, NAME ## Tag> { \
        template<typename Container> \
        static constexpr NAME ## Encoding<Container> test(const NAME ## Encoding<Container>*); \
        using type = decltype(test(std::declval<PinData*>())); \
    }; \
	template<typename PinData> using          has_ ## FIELD ## _encoding = has_pin_data_encoding_t<PinData, NAME ## Tag>; \
	template<typename PinData> constexpr bool has_ ## FIELD ## _encoding_v = has_pin_data_encoding_t<PinData, NAME ## Tag>::value; \
	template<typename Tag, typename PinData> std::enable_if_t<std::is_same_v<Tag, NAME ## Tag>, get_pin_data_container_t<PinData, NAME ## Tag>&> GetContainer(PinData& pinData) { return pinData.FIELD; } \
	template<typename Tag, typename PinData> std::enable_if_t<std::is_same_v<Tag, NAME ## Tag>, get_pin_data_container_t<PinData, NAME ## Tag> const&> GetContainer(PinData const& pinData) { return pinData.FIELD; } \

DEFINE_PINDATA_ENCODING(Good, good)         // Normal circuit version 1
DEFINE_PINDATA_ENCODING(Bad, bad)           // Faulty circuit version 1
DEFINE_PINDATA_ENCODING(Bad2, bad2)         // Faulty circuit version 2
DEFINE_PINDATA_ENCODING(Diff, diff)         // Fault difference version 1
DEFINE_PINDATA_ENCODING(Diff2, diff2)       // Fault difference version 2
DEFINE_PINDATA_ENCODING(LDiff, ldiff)       // Logic difference version 1
DEFINE_PINDATA_ENCODING(LDiff2, ldiff2)     // Logic difference version 2
DEFINE_PINDATA_ENCODING(Switch, switching)  // Switching activity
DEFINE_PINDATA_ENCODING(Path, path)         // Sensitized path for path ATPG

DEFINE_PINDATA_TAG(FaultLocation, fault_location)                       // Fault location version 1
DEFINE_PINDATA_TAG(FaultLocation2, fault_location2)                     // Fault location version 2
DEFINE_PINDATA_TAG(FaultInputCone, fault_input_cone)                    // Fault primary input cone version 1
DEFINE_PINDATA_TAG(FaultInputCone2, fault_input_cone2)                  // Fault primary input cone version 2
DEFINE_PINDATA_TAG(FaultOutputCone, fault_output_cone)                  // Fault primary output cone version 1
DEFINE_PINDATA_TAG(FaultOutputCone2, fault_output_cone2)                // Fault primary output cone version 2
DEFINE_PINDATA_TAG(FaultSupportInputCone, fault_support_input_cone)     // Fault secondary input cone version 1
DEFINE_PINDATA_TAG(FaultSupportInputCone2, fault_support_input_cone2)   // Fault secondary input cone version 2
DEFINE_PINDATA_TAG(FaultSupportOutputCone, fault_support_output_cone)   // Fault secondary output cone version 1
DEFINE_PINDATA_TAG(FaultSupportOutputCone2, fault_support_output_cone2) // Fault secondary output cone version 2

// ----------------------------------------------------------------------------
// Predefined pin data containers
// ----------------------------------------------------------------------------

template<typename PinData, typename Tag> struct PinDataTagContainer { };
#define ATTACH_PINDATA_TAG(NAME, TAG, CONTAINER) \
	template<typename LogicContainer> struct PinDataTagContainer<PinData ## NAME<LogicContainer>, TAG ## Tag>: \
	public TAG ## Tag { using LogicContainer ## TAG ## Type = CONTAINER; using Has ## TAG ## Element = std::false_type; };
#define ATTACH_PINDATA_ENCODING(NAME, TAG, CONTAINER) \
	template<typename LogicContainer> struct PinDataTagContainer<PinData ## NAME<LogicContainer>, TAG ## Tag>: \
	public TAG ## Tag, public TAG ## Encoding<CONTAINER> { using LogicContainer ## TAG ## Type = CONTAINER; using Has ## TAG ## Element = std::true_type; };

template<typename PinData, typename... C> struct get_pin_data_name { };
template<typename PinData, typename... C> constexpr auto get_pin_data_name_v = get_pin_data_name<PinData, C...>::value;

template<typename PinData, typename... Ts> struct PinDataTagsImpl: public PinDataTagContainer<PinData, Ts>... { };
#define DEFINE_PINDATA(NAME, ...) \
	template<typename LogicContainer> struct PinData ## NAME; \
	template<typename LogicContainer> struct PinData ## NAME: public PinDataTagsImpl<PinData ## NAME<LogicContainer>, __VA_ARGS__> { }; \
	template<typename... C> struct get_pin_data_name<PinData ## NAME<C...>> { static constexpr const char* value = #NAME; };

DEFINE_PINDATA(G, GoodTag)
	ATTACH_PINDATA_ENCODING(G, Good, LogicContainer)

DEFINE_PINDATA(GB, GoodTag, BadTag, DiffTag, LDiffTag)
	ATTACH_PINDATA_ENCODING(GB, Good, LogicContainer)
	ATTACH_PINDATA_ENCODING(GB, Bad, LogicContainer)
	ATTACH_PINDATA_TAG(GB, Diff, LogicContainer01)
	ATTACH_PINDATA_TAG(GB, LDiff, LogicContainer01)

DEFINE_PINDATA(GBD, GoodTag, BadTag, DiffTag, LDiffTag)
	ATTACH_PINDATA_ENCODING(GBD, Good, LogicContainer)
	ATTACH_PINDATA_ENCODING(GBD, Bad, LogicContainer)
	ATTACH_PINDATA_ENCODING(GBD, Diff, LogicContainer01)
	ATTACH_PINDATA_TAG(GBD, LDiff, LogicContainer01)

DEFINE_PINDATA(GBP, GoodTag, BadTag, DiffTag, LDiffTag, PathTag)
	ATTACH_PINDATA_ENCODING(GBP, Good, LogicContainer)
	ATTACH_PINDATA_ENCODING(GBP, Bad, LogicContainer)
	ATTACH_PINDATA_ENCODING(GBP, Path, LogicContainer01)
	ATTACH_PINDATA_TAG(GBP, Diff, LogicContainer01)
	ATTACH_PINDATA_TAG(GBP, LDiff, LogicContainer01)

DEFINE_PINDATA(GBDP, GoodTag, BadTag, DiffTag, LDiffTag, PathTag)
	ATTACH_PINDATA_ENCODING(GBDP, Good, LogicContainer)
	ATTACH_PINDATA_ENCODING(GBDP, Bad, LogicContainer)
	ATTACH_PINDATA_ENCODING(GBDP, Diff, LogicContainer01)
	ATTACH_PINDATA_ENCODING(GBDP, Path, LogicContainer01)
	ATTACH_PINDATA_TAG(GBDP, LDiff, LogicContainer01)

DEFINE_PINDATA(GB2, GoodTag, BadTag, DiffTag, LDiffTag, Bad2Tag, Diff2Tag, LDiff2Tag)
	ATTACH_PINDATA_ENCODING(GB2, Good, LogicContainer)
	ATTACH_PINDATA_ENCODING(GB2, Bad, LogicContainer)
	ATTACH_PINDATA_ENCODING(GB2, Bad2, LogicContainer)
	ATTACH_PINDATA_TAG(GB2, Diff, LogicContainer01)
	ATTACH_PINDATA_TAG(GB2, Diff2, LogicContainer01)
	ATTACH_PINDATA_TAG(GB2, LDiff, LogicContainer01)
	ATTACH_PINDATA_TAG(GB2, LDiff2, LogicContainer01)

DEFINE_PINDATA(GBD2, GoodTag, BadTag, DiffTag, LDiffTag, Bad2Tag, Diff2Tag, LDiff2Tag)
	ATTACH_PINDATA_ENCODING(GBD2, Good, LogicContainer)
	ATTACH_PINDATA_ENCODING(GBD2, Bad, LogicContainer)
	ATTACH_PINDATA_ENCODING(GBD2, Bad2, LogicContainer)
	ATTACH_PINDATA_ENCODING(GBD2, Diff, LogicContainer01)
	ATTACH_PINDATA_ENCODING(GBD2, Diff2, LogicContainer01)
	ATTACH_PINDATA_TAG(GBD2, LDiff, LogicContainer01)
	ATTACH_PINDATA_TAG(GBD2, LDiff2, LogicContainer01)

DEFINE_PINDATA(GSwitch, GoodTag, SwitchTag)
	ATTACH_PINDATA_ENCODING(GSwitch, Good, LogicContainer)
	ATTACH_PINDATA_ENCODING(GSwitch, Switch, LogicContainer01)

// -----------------------------------------------------------------------------
// The following macros are used to explicitly instantiate all the TPG modules.
// This will not be necessary once everything is converted to C++ 20 modules.
// -----------------------------------------------------------------------------

// Define the macro FOR_CONTAINER(PINDATA) and do the work in there
#define INSTANTIATE_FOR_ALL_PINDATA_CONTAINER(PINDATA) \
	FOR_PINDATA(PINDATA<::FreiTest::Tpg::LogicContainer01>) \
	FOR_PINDATA(PINDATA<::FreiTest::Tpg::LogicContainer01X>) \
	FOR_PINDATA(PINDATA<::FreiTest::Tpg::LogicContainer01F>) \
	FOR_PINDATA(PINDATA<::FreiTest::Tpg::LogicContainerU01X>) \
	FOR_PINDATA(PINDATA<::FreiTest::Tpg::LogicContainerU01F>)

#define INSTANTIATE_FOR_ALL_PINDATA \
	INSTANTIATE_FOR_ALL_PINDATA_CONTAINER(::FreiTest::Tpg::PinDataG) \
	INSTANTIATE_FOR_ALL_PINDATA_CONTAINER(::FreiTest::Tpg::PinDataGB) \
	INSTANTIATE_FOR_ALL_PINDATA_CONTAINER(::FreiTest::Tpg::PinDataGBD) \
	INSTANTIATE_FOR_ALL_PINDATA_CONTAINER(::FreiTest::Tpg::PinDataGBP) \
	INSTANTIATE_FOR_ALL_PINDATA_CONTAINER(::FreiTest::Tpg::PinDataGBDP) \
	INSTANTIATE_FOR_ALL_PINDATA_CONTAINER(::FreiTest::Tpg::PinDataGBD2) \
	INSTANTIATE_FOR_ALL_PINDATA_CONTAINER(::FreiTest::Tpg::PinDataGSwitch)

// Define the macro FOR_TAG(PINDATA, TAG) and do the work in there
#define INSTANTIATE_FOR_ALL_TAGS_CONTAINER(PINDATA, TAG) \
	FOR_TAG(PINDATA<::FreiTest::Tpg::LogicContainer01>, TAG) \
	FOR_TAG(PINDATA<::FreiTest::Tpg::LogicContainer01X>, TAG) \
	FOR_TAG(PINDATA<::FreiTest::Tpg::LogicContainer01F>, TAG) \
	FOR_TAG(PINDATA<::FreiTest::Tpg::LogicContainerU01X>, TAG) \
	FOR_TAG(PINDATA<::FreiTest::Tpg::LogicContainerU01F>, TAG)

#define INSTANTIATE_FOR_ALL_TAGS \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataG, ::FreiTest::Tpg::GoodTag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGB, ::FreiTest::Tpg::GoodTag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGB, ::FreiTest::Tpg::BadTag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGB, ::FreiTest::Tpg::DiffTag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGB, ::FreiTest::Tpg::LDiffTag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGBD, ::FreiTest::Tpg::GoodTag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGBD, ::FreiTest::Tpg::BadTag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGBD, ::FreiTest::Tpg::DiffTag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGBD, ::FreiTest::Tpg::LDiffTag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGBP, ::FreiTest::Tpg::GoodTag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGBP, ::FreiTest::Tpg::BadTag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGBP, ::FreiTest::Tpg::DiffTag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGBP, ::FreiTest::Tpg::LDiffTag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGBP, ::FreiTest::Tpg::PathTag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGBDP, ::FreiTest::Tpg::GoodTag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGBDP, ::FreiTest::Tpg::BadTag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGBDP, ::FreiTest::Tpg::DiffTag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGBDP, ::FreiTest::Tpg::LDiffTag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGBDP, ::FreiTest::Tpg::PathTag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGBD2, ::FreiTest::Tpg::GoodTag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGBD2, ::FreiTest::Tpg::BadTag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGBD2, ::FreiTest::Tpg::DiffTag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGBD2, ::FreiTest::Tpg::LDiffTag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGBD2, ::FreiTest::Tpg::Bad2Tag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGBD2, ::FreiTest::Tpg::Diff2Tag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGBD2, ::FreiTest::Tpg::LDiff2Tag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGSwitch, ::FreiTest::Tpg::GoodTag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGSwitch, ::FreiTest::Tpg::SwitchTag)

#define INSTANTIATE_FOR_ALL_ENCODINGS \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataG, ::FreiTest::Tpg::GoodTag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGB, ::FreiTest::Tpg::GoodTag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGB, ::FreiTest::Tpg::BadTag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGBD, ::FreiTest::Tpg::GoodTag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGBD, ::FreiTest::Tpg::BadTag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGBD, ::FreiTest::Tpg::DiffTag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGBP, ::FreiTest::Tpg::GoodTag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGBP, ::FreiTest::Tpg::BadTag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGBP, ::FreiTest::Tpg::PathTag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGBDP, ::FreiTest::Tpg::GoodTag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGBDP, ::FreiTest::Tpg::BadTag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGBDP, ::FreiTest::Tpg::DiffTag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGBDP, ::FreiTest::Tpg::PathTag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGBD2, ::FreiTest::Tpg::GoodTag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGBD2, ::FreiTest::Tpg::BadTag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGBD2, ::FreiTest::Tpg::DiffTag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGBD2, ::FreiTest::Tpg::Bad2Tag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGBD2, ::FreiTest::Tpg::Diff2Tag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGSwitch, ::FreiTest::Tpg::GoodTag) \
	INSTANTIATE_FOR_ALL_TAGS_CONTAINER(::FreiTest::Tpg::PinDataGSwitch, ::FreiTest::Tpg::SwitchTag)

// Define the macro FOR_TAG(PINDATA, TAG) and do the work in there
#define INSTANTIATE_FOR_ALL_TAGGING_TAG(PINDATA) \
	FOR_TAG(PINDATA, ::FreiTest::Tpg::GoodTag) \
	FOR_TAG(PINDATA, ::FreiTest::Tpg::BadTag) \
	FOR_TAG(PINDATA, ::FreiTest::Tpg::Bad2Tag) \
	FOR_TAG(PINDATA, ::FreiTest::Tpg::DiffTag) \
	FOR_TAG(PINDATA, ::FreiTest::Tpg::Diff2Tag) \
	FOR_TAG(PINDATA, ::FreiTest::Tpg::LDiffTag) \
	FOR_TAG(PINDATA, ::FreiTest::Tpg::LDiff2Tag) \
	FOR_TAG(PINDATA, ::FreiTest::Tpg::PathTag) \
	FOR_TAG(PINDATA, ::FreiTest::Tpg::SwitchTag) \
	FOR_TAG(PINDATA, ::FreiTest::Tpg::FaultLocationTag) \
	FOR_TAG(PINDATA, ::FreiTest::Tpg::FaultLocation2Tag) \
	FOR_TAG(PINDATA, ::FreiTest::Tpg::FaultInputConeTag) \
	FOR_TAG(PINDATA, ::FreiTest::Tpg::FaultInputCone2Tag) \
	FOR_TAG(PINDATA, ::FreiTest::Tpg::FaultOutputConeTag) \
	FOR_TAG(PINDATA, ::FreiTest::Tpg::FaultOutputCone2Tag) \
	FOR_TAG(PINDATA, ::FreiTest::Tpg::FaultSupportInputConeTag) \
	FOR_TAG(PINDATA, ::FreiTest::Tpg::FaultSupportInputCone2Tag) \
	FOR_TAG(PINDATA, ::FreiTest::Tpg::FaultSupportOutputConeTag) \
	FOR_TAG(PINDATA, ::FreiTest::Tpg::FaultSupportOutputCone2Tag)

#define INSTANTIATE_FOR_ALL_TAGGING_CONTAINER(PINDATA) \
	INSTANTIATE_FOR_ALL_TAGGING_TAG(PINDATA<::FreiTest::Tpg::LogicContainer01>) \
	INSTANTIATE_FOR_ALL_TAGGING_TAG(PINDATA<::FreiTest::Tpg::LogicContainer01X>) \
	INSTANTIATE_FOR_ALL_TAGGING_TAG(PINDATA<::FreiTest::Tpg::LogicContainer01F>) \
	INSTANTIATE_FOR_ALL_TAGGING_TAG(PINDATA<::FreiTest::Tpg::LogicContainerU01X>) \
	INSTANTIATE_FOR_ALL_TAGGING_TAG(PINDATA<::FreiTest::Tpg::LogicContainerU01F>)

#define INSTANTIATE_FOR_ALL_TAGGING \
	INSTANTIATE_FOR_ALL_TAGGING_CONTAINER(::FreiTest::Tpg::PinDataG) \
	INSTANTIATE_FOR_ALL_TAGGING_CONTAINER(::FreiTest::Tpg::PinDataGB) \
	INSTANTIATE_FOR_ALL_TAGGING_CONTAINER(::FreiTest::Tpg::PinDataGBD) \
	INSTANTIATE_FOR_ALL_TAGGING_CONTAINER(::FreiTest::Tpg::PinDataGBP) \
	INSTANTIATE_FOR_ALL_TAGGING_CONTAINER(::FreiTest::Tpg::PinDataGBDP) \
	INSTANTIATE_FOR_ALL_TAGGING_CONTAINER(::FreiTest::Tpg::PinDataGBD2) \
	INSTANTIATE_FOR_ALL_TAGGING_CONTAINER(::FreiTest::Tpg::PinDataGSwitch)

// ----------------------------------------------------------------------------
// Sanity checks
// ----------------------------------------------------------------------------

static_assert( has_pin_data_tag_v<PinDataG<LogicContainer01X>, GoodTag>);
static_assert(!has_pin_data_tag_v<PinDataG<LogicContainer01X>, BadTag>);
static_assert(!has_pin_data_tag_v<PinDataG<LogicContainer01X>, DiffTag>);
static_assert( has_pin_data_encoding_v<PinDataG<LogicContainer01X>, GoodTag>);
static_assert(!has_pin_data_encoding_v<PinDataG<LogicContainer01X>, BadTag>);
static_assert(!has_pin_data_encoding_v<PinDataG<LogicContainer01X>, DiffTag>);
static_assert(std::is_same_v<get_pin_data_encoding_t<PinDataG<LogicContainer01X>, GoodTag>, GoodEncoding<LogicContainer01X>>);
static_assert(std::is_same_v<get_pin_data_container_t<PinDataG<LogicContainer01X>, GoodTag>, LogicContainer01X>);

static_assert( has_pin_data_tag_v<PinDataGB<LogicContainer01X>, GoodTag>);
static_assert( has_pin_data_tag_v<PinDataGB<LogicContainer01X>, BadTag>);
static_assert( has_pin_data_tag_v<PinDataGB<LogicContainer01X>, DiffTag>);
static_assert( has_pin_data_encoding_v<PinDataGB<LogicContainer01X>, GoodTag>);
static_assert( has_pin_data_encoding_v<PinDataGB<LogicContainer01X>, BadTag>);
static_assert(!has_pin_data_encoding_v<PinDataGB<LogicContainer01X>, DiffTag>);
static_assert(std::is_same_v<get_pin_data_encoding_t<PinDataGB<LogicContainer01X>, GoodTag>, GoodEncoding<LogicContainer01X>>);
static_assert(std::is_same_v<get_pin_data_encoding_t<PinDataGB<LogicContainer01X>, BadTag>, BadEncoding<LogicContainer01X>>);
static_assert(std::is_same_v<get_pin_data_container_t<PinDataGB<LogicContainer01X>, GoodTag>, LogicContainer01X>);
static_assert(std::is_same_v<get_pin_data_container_t<PinDataGB<LogicContainer01X>, BadTag>, LogicContainer01X>);

static_assert( has_pin_data_tag_v<PinDataGBD<LogicContainer01X>, GoodTag>);
static_assert( has_pin_data_tag_v<PinDataGBD<LogicContainer01X>, BadTag>);
static_assert( has_pin_data_tag_v<PinDataGBD<LogicContainer01X>, DiffTag>);
static_assert( has_pin_data_encoding_v<PinDataGBD<LogicContainer01X>, DiffTag>);
static_assert( has_pin_data_encoding_v<PinDataGBD<LogicContainer01X>, BadTag>);
static_assert( has_pin_data_encoding_v<PinDataGBD<LogicContainer01X>, DiffTag>);
static_assert(std::is_same_v<get_pin_data_encoding_t<PinDataGBD<LogicContainer01X>, GoodTag>, GoodEncoding<LogicContainer01X>>);
static_assert(std::is_same_v<get_pin_data_encoding_t<PinDataGBD<LogicContainer01X>, BadTag>, BadEncoding<LogicContainer01X>>);
static_assert(std::is_same_v<get_pin_data_container_t<PinDataGBD<LogicContainer01X>, GoodTag>, LogicContainer01X>);
static_assert(std::is_same_v<get_pin_data_container_t<PinDataGBD<LogicContainer01X>, BadTag>, LogicContainer01X>);

static_assert( has_pin_data_tag_v<PinDataGSwitch<LogicContainer01X>, GoodTag>);
static_assert( has_pin_data_tag_v<PinDataGSwitch<LogicContainer01X>, SwitchTag>);
static_assert( has_pin_data_encoding_v<PinDataGSwitch<LogicContainer01X>, GoodTag>);
static_assert( has_pin_data_encoding_v<PinDataGSwitch<LogicContainer01X>, SwitchTag>);
static_assert(std::is_same_v<get_pin_data_encoding_t<PinDataGSwitch<LogicContainer01X>, GoodTag>, GoodEncoding<LogicContainer01X>>);
static_assert(std::is_same_v<get_pin_data_container_t<PinDataGSwitch<LogicContainer01X>, GoodTag>, LogicContainer01X>);
static_assert(std::is_same_v<get_pin_data_encoding_t<PinDataGSwitch<LogicContainer01X>, SwitchTag>, SwitchEncoding<LogicContainer01>>);
static_assert(std::is_same_v<get_pin_data_container_t<PinDataGSwitch<LogicContainer01X>, SwitchTag>, LogicContainer01>);

};
};
