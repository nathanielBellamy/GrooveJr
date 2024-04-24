package dev.nateschieber.groovesprings.services.entities;

import dev.nateschieber.groovesprings.entities.Album;
import dev.nateschieber.groovesprings.entities.Artist;
import dev.nateschieber.groovesprings.entities.Track;
import dev.nateschieber.groovesprings.repositories.TrackRepository;
import dev.nateschieber.groovesprings.rest.dtos.track.TrackCreateDto;
import dev.nateschieber.groovesprings.rest.dtos.track.TrackEntityDto;
import java.time.LocalDate;
import java.util.List;
import java.util.Optional;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

@Service
public class TrackService implements ITrackService<Track, TrackEntityDto, TrackCreateDto> {
  private final AlbumService albumService;
  private final ArtistService artistService;
  private final TrackRepository trackRepository;

  @Autowired
  public TrackService(
      AlbumService albumService,
      ArtistService artistService,
      TrackRepository trackRepository) {
    this.albumService = albumService;
    this.artistService = artistService;
    this.trackRepository = trackRepository;
  }

  @Override
  public List<Track> findAll() {
    return this.trackRepository.findAll();
  }

  @Override
  public Optional<Track> findById(Long id) {
    return this.trackRepository.findById(id);
  }

  @Override
  public void deleteById(Long id) {
    this.trackRepository.deleteById(id);
  }

  @Override
  public Track update(Long id, TrackEntityDto dto) {
    Track updatedTrack = new Track(id, dto);
    return this.trackRepository.save(updatedTrack);
  }

  @Override
  public Track createFromDto(TrackCreateDto dto) {
    Optional<Album> album = albumService.findById(dto.albumId());
    List<Artist> artists = artistService.findAllById(dto.artistIds());
    Track track = new Track(artists, album, dto.title(), dto.trackNumber(), dto.duration(), dto.mediaType(), dto.releaseDate());
    return trackRepository.save(track);
  }

  @Override
  public Track save(Track track, long artistId) {
    Track savedTrack = this.trackRepository.save(track);

    return savedTrack;
  }

  @Override
  public List<Track> findByReleaseYear(int year) {
    return trackRepository.findByReleaseDateBetween(
        LocalDate.of(year, 1, 1),
        LocalDate.of(year, 12, 31)
    );
  }
}