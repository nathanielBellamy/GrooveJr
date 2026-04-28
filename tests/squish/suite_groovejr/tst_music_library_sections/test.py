def main():
    source(findFile("scripts", "common.py"))

    start_groovejr()

    verify_visible(filters_header(), "Filters header should be visible.")
    verify_visible(album_header(), "Albums header should be visible.")
    verify_visible(artist_header(), "Artists header should be visible.")
    verify_visible(genre_header(), "Genres header should be visible.")
    verify_visible(playlist_header(), "Playlists header should be visible.")
    verify_visible(files_button(), "Files button should be visible.")
    verify_visible(queue_button(), "Queue button should be visible.")
    verify_visible(cache_button(), "Cache button should be visible.")

    verify_visible(audio_file_table(), "Files table should be the default visible section.")
    verify_hidden(queue_table(), "Queue table should be hidden by default.")
    verify_hidden(cache_table(), "Cache table should be hidden by default.")

    click_named_button(queue_button())
    verify_hidden(audio_file_table(), "Files table should hide after selecting Queue.")
    verify_visible(queue_table(), "Queue table should show after selecting Queue.")
    verify_hidden(cache_table(), "Cache table should remain hidden after selecting Queue.")

    click_named_button(cache_button())
    verify_hidden(audio_file_table(), "Files table should stay hidden after selecting Cache.")
    verify_hidden(queue_table(), "Queue table should hide after selecting Cache.")
    verify_visible(cache_table(), "Cache table should show after selecting Cache.")

    click_named_button(files_button())
    verify_visible(audio_file_table(), "Files table should show again after reselecting Files.")
    verify_hidden(queue_table(), "Queue table should hide again after reselecting Files.")
    verify_hidden(cache_table(), "Cache table should hide again after reselecting Files.")
