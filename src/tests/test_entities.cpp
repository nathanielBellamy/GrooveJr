#include <gtest/gtest.h>

#include "db/entity/mixer/ChannelEntity.h"
#include "db/entity/mixer/Scene.h"
#include "db/entity/mixer/Plugin.h"
#include "db/entity/musicLibrary/Artist.h"
#include "db/entity/musicLibrary/Album.h"
#include "db/entity/musicLibrary/Genre.h"
#include "db/entity/musicLibrary/Track.h"
#include "db/entity/musicLibrary/Playlist.h"
#include "db/entity/AppStateEntity.h"

using namespace Gj;
using namespace Gj::Db;

// ═══════════════════════════════════════════════════════
// Scene
// ═══════════════════════════════════════════════════════

TEST(SceneTest, Base_DefaultValues) {
    auto scene = Scene::base();
    EXPECT_EQ(scene.id, 0u);
    EXPECT_EQ(scene.sceneId, 0u);
    EXPECT_FLOAT_EQ(scene.playbackSpeed, 1.0f);
    EXPECT_EQ(scene.version, 0u);
}

TEST(SceneTest, Base_Name) {
    auto scene = Scene::base();
    std::string name = scene.name.std_str();
    EXPECT_NE(name.find("Blank Scene"), std::string::npos);
}

TEST(SceneTest, Constructor_Full) {
    Scene scene(42, 7, AtomicStr("My Scene"), 1.5f, 3);
    EXPECT_EQ(scene.id, 42u);
    EXPECT_EQ(scene.sceneId, 7u);
    EXPECT_FLOAT_EQ(scene.playbackSpeed, 1.5f);
    EXPECT_EQ(scene.version, 3u);
}

TEST(SceneTest, Constructor_Minimal) {
    Scene scene(AtomicStr("Quick Scene"), 0.75f);
    EXPECT_EQ(scene.id, 0u);
    EXPECT_EQ(scene.sceneId, 0u);
    EXPECT_FLOAT_EQ(scene.playbackSpeed, 0.75f);
}

TEST(SceneTest, ToStdString_ContainsFields) {
    Scene scene(1, 2, AtomicStr("Test"), 1.0f, 0);
    std::string str = scene.toStdString();
    EXPECT_NE(str.find("Scene"), std::string::npos);
    EXPECT_NE(str.find("dbId: 1"), std::string::npos);
    EXPECT_NE(str.find("sceneId: 2"), std::string::npos);
}

// ═══════════════════════════════════════════════════════
// ChannelEntity
// ═══════════════════════════════════════════════════════

TEST(ChannelEntityTest, Constructor) {
    ChannelEntity ch(
        1, 0, AtomicStr("Main"),
        1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f
    );
    EXPECT_EQ(ch.id, 1u);
    EXPECT_EQ(ch.channelIndex, 0u);
    EXPECT_FLOAT_EQ(ch.gain, 1.0f);
    EXPECT_FLOAT_EQ(ch.mute, 0.0f);
    EXPECT_FLOAT_EQ(ch.panL, -1.0f);
    EXPECT_FLOAT_EQ(ch.panR, 1.0f);
}

TEST(ChannelEntityTest, BaseChannels_ReturnsTwoChannels) {
    auto channels = ChannelEntity::baseChannels();
    EXPECT_EQ(channels.size(), 2u);
}

TEST(ChannelEntityTest, BaseChannels_MainChannel) {
    auto channels = ChannelEntity::baseChannels();
    const auto& main = channels[0];
    EXPECT_EQ(main.channelIndex, 0u);
    EXPECT_FLOAT_EQ(main.gain, 1.0f);
    EXPECT_FLOAT_EQ(main.mute, 0.0f);
    EXPECT_FLOAT_EQ(main.solo, 0.0f);
    EXPECT_FLOAT_EQ(main.pan, 0.0f);
    EXPECT_FLOAT_EQ(main.gainL, 1.0f);
    EXPECT_FLOAT_EQ(main.gainR, 1.0f);
    EXPECT_FLOAT_EQ(main.panL, -1.0f);
    EXPECT_FLOAT_EQ(main.panR, 1.0f);
}

TEST(ChannelEntityTest, BaseChannels_Channel1) {
    auto channels = ChannelEntity::baseChannels();
    const auto& ch1 = channels[1];
    EXPECT_EQ(ch1.channelIndex, 1u);
    EXPECT_FLOAT_EQ(ch1.gain, 1.0f);
}

TEST(ChannelEntityTest, OperatorEquals_SameId) {
    ChannelEntity a(1, 0, AtomicStr("A"), 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, -1, 1);
    ChannelEntity b(1, 1, AtomicStr("B"), 0.5f, 0, 0, 0, 1, 1, 0, 0, 0, 0, -1, 1);
    EXPECT_TRUE(a = b);
}

TEST(ChannelEntityTest, OperatorEquals_DifferentId) {
    ChannelEntity a(1, 0, AtomicStr("A"), 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, -1, 1);
    ChannelEntity b(2, 0, AtomicStr("A"), 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, -1, 1);
    EXPECT_FALSE(a = b);
}

TEST(ChannelEntityTest, OperatorLessThan) {
    ChannelEntity a(0, 0, AtomicStr("A"), 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, -1, 1);
    ChannelEntity b(0, 1, AtomicStr("B"), 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, -1, 1);
    EXPECT_TRUE(a < b);
    EXPECT_FALSE(b < a);
}

// ═══════════════════════════════════════════════════════
// Artist
// ═══════════════════════════════════════════════════════

TEST(ArtistTest, Constructor_WithId) {
    Artist artist(42, AtomicStr("Pink Floyd"));
    EXPECT_EQ(artist.id, 42u);
}

TEST(ArtistTest, Constructor_WithoutId) {
    Artist artist(AtomicStr("Pink Floyd"));
    EXPECT_EQ(artist.id, 0u);
}

TEST(ArtistTest, Unknown) {
    auto artist = Artist::unknown();
    EXPECT_EQ(artist.id, 0u);
    std::string name = artist.name.std_str();
    EXPECT_NE(name.find("Unknown Artist"), std::string::npos);
}

// ═══════════════════════════════════════════════════════
// Album
// ═══════════════════════════════════════════════════════

TEST(AlbumTest, Constructor_TitleOnly) {
    Album album(AtomicStr("Dark Side of the Moon"));
    EXPECT_EQ(album.id, 0u);
    EXPECT_EQ(album.year, 0u);
}

TEST(AlbumTest, Constructor_TitleAndYear) {
    Album album(AtomicStr("Wish You Were Here"), 1975);
    EXPECT_EQ(album.id, 0u);
    EXPECT_EQ(album.year, 1975u);
}

TEST(AlbumTest, Constructor_Full) {
    Album album(10, AtomicStr("Animals"), 1977);
    EXPECT_EQ(album.id, 10u);
    EXPECT_EQ(album.year, 1977u);
}

// ═══════════════════════════════════════════════════════
// Genre
// ═══════════════════════════════════════════════════════

TEST(GenreTest, Constructor_NameOnly) {
    Genre genre(AtomicStr("Rock"));
    EXPECT_EQ(genre.id, 0u);
}

TEST(GenreTest, Constructor_Full) {
    Genre genre(5, AtomicStr("Jazz"));
    EXPECT_EQ(genre.id, 5u);
}

// ═══════════════════════════════════════════════════════
// Track
// ═══════════════════════════════════════════════════════

TEST(TrackTest, DefaultConstructor) {
    Track track;
    EXPECT_EQ(track.id, 0u);
    EXPECT_EQ(track.albumId, 0u);
    EXPECT_EQ(track.trackNumber, 0);
}

TEST(TrackTest, Constructor_Full) {
    Track track(1, 2, AtomicStr("Comfortably Numb"), 6);
    EXPECT_EQ(track.id, 1u);
    EXPECT_EQ(track.albumId, 2u);
    EXPECT_EQ(track.trackNumber, 6);
}

TEST(TrackTest, Constructor_WithoutId) {
    Track track(2, AtomicStr("Time"), 4);
    EXPECT_EQ(track.id, 0u);
    EXPECT_EQ(track.albumId, 2u);
    EXPECT_EQ(track.trackNumber, 4);
}

TEST(TrackTest, Empty) {
    auto track = Track::empty();
    EXPECT_EQ(track.id, 0u);
    EXPECT_EQ(track.trackNumber, 0);
    std::string title = track.title.std_str();
    EXPECT_NE(title.find("Empty Track"), std::string::npos);
}

// ═══════════════════════════════════════════════════════
// Playlist
// ═══════════════════════════════════════════════════════

TEST(PlaylistTest, Constructor_Full) {
    Playlist pl(1, AtomicStr("My Favorites"));
    EXPECT_EQ(pl.id, 1u);
}

TEST(PlaylistTest, Constructor_NameOnly) {
    Playlist pl(AtomicStr("Road Trip"));
    EXPECT_EQ(pl.id, 0u);
}

TEST(PlaylistTest, Empty) {
    auto pl = Playlist::empty();
    EXPECT_EQ(pl.id, 0u);
    std::string name = pl.name.std_str();
    EXPECT_NE(name.find("Empty Playlist"), std::string::npos);
}

// ═══════════════════════════════════════════════════════
// Plugin
// ═══════════════════════════════════════════════════════

TEST(PluginTest, Constructor_Full) {
    Plugin p(
        1, AtomicStr("/path/to/plugin.vst3"), AtomicStr("VST3"),
        AtomicStr("Reverb"), 0, 0, true, {}, {}, {}, {}
    );
    EXPECT_EQ(p.id, 1u);
    EXPECT_EQ(p.channelIndex, 0u);
    EXPECT_EQ(p.pluginIndex, 0u);
    EXPECT_TRUE(p.enabled);
}

TEST(PluginTest, Constructor_WithoutId) {
    Plugin p(
        AtomicStr("/path/to/plugin.vst3"), AtomicStr("VST3"),
        AtomicStr("Delay"), 1, 2, false, {}, {}, {}, {}
    );
    EXPECT_EQ(p.id, 0u);
    EXPECT_EQ(p.channelIndex, 1u);
    EXPECT_EQ(p.pluginIndex, 2u);
    EXPECT_FALSE(p.enabled);
}

TEST(PluginTest, OperatorEquals_SameId) {
    Plugin a(1, AtomicStr("a"), AtomicStr("VST3"), AtomicStr("A"), 0, 0, true, {}, {}, {}, {});
    Plugin b(1, AtomicStr("b"), AtomicStr("VST3"), AtomicStr("B"), 1, 1, false, {}, {}, {}, {});
    EXPECT_TRUE(a = b);
}

TEST(PluginTest, OperatorEquals_DifferentId) {
    Plugin a(1, AtomicStr("a"), AtomicStr("VST3"), AtomicStr("A"), 0, 0, true, {}, {}, {}, {});
    Plugin b(2, AtomicStr("b"), AtomicStr("VST3"), AtomicStr("B"), 0, 0, true, {}, {}, {}, {});
    EXPECT_FALSE(a = b);
}

TEST(PluginTest, OperatorLessThan) {
    Plugin a(0, AtomicStr("a"), AtomicStr("VST3"), AtomicStr("A"), 0, 0, true, {}, {}, {}, {});
    Plugin b(0, AtomicStr("b"), AtomicStr("VST3"), AtomicStr("B"), 0, 1, true, {}, {}, {}, {});
    EXPECT_TRUE(a < b);
    EXPECT_FALSE(b < a);
}

TEST(PluginTest, BlobStorage) {
    std::vector<uint8_t> blob = {0x01, 0x02, 0x03, 0xFF};
    Plugin p(1, AtomicStr("p"), AtomicStr("VST3"), AtomicStr("Test"), 0, 0, true,
             blob, {}, blob, {});
    EXPECT_EQ(p.audioHostComponentStateBlob.size(), 4u);
    EXPECT_EQ(p.audioHostComponentStateBlob[3], 0xFF);
    EXPECT_EQ(p.editorHostComponentStateBlob.size(), 4u);
    EXPECT_TRUE(p.audioHostControllerStateBlob.empty());
}

// ═══════════════════════════════════════════════════════
// AppStateEntity
// ═══════════════════════════════════════════════════════

TEST(AppStateEntityTest, Constructor) {
    AppStateEntity e(1, 256, 0, 10000, 0);
    EXPECT_EQ(e.id, 1u);
    EXPECT_EQ(e.audioFramesPerBuffer, 256u);
    EXPECT_EQ(e.sceneId, 0u);
    EXPECT_EQ(e.crossfade, 10000);
    EXPECT_EQ(e.currentlyPlaying, 0u);
}

TEST(AppStateEntityTest, Base) {
    auto e = AppStateEntity::base();
    EXPECT_EQ(e.id, 0u);
    EXPECT_EQ(e.audioFramesPerBuffer, 256u);
    EXPECT_EQ(e.crossfade, 10000);
}
