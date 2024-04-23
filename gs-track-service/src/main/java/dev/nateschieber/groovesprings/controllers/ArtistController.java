package dev.nateschieber.groovesprings.controllers;

import dev.nateschieber.groovesprings.entities.Artist;
import dev.nateschieber.groovesprings.helpers.HttpHelper;
import dev.nateschieber.groovesprings.rest.dtos.artist.ArtistCreateDto;
import dev.nateschieber.groovesprings.rest.dtos.artist.ArtistEntityDto;
import dev.nateschieber.groovesprings.rest.responses.artist.ArtistDeleteResponse;
import dev.nateschieber.groovesprings.rest.responses.artist.ArtistEntityResponse;
import dev.nateschieber.groovesprings.rest.responses.artist.ArtistGetAllResponse;
import dev.nateschieber.groovesprings.services.ArtistService;
import java.net.URI;
import java.util.List;
import java.util.Optional;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.MediaType;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.DeleteMapping;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.PutMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

@RestController
@RequestMapping("api/v1/artists")
public class ArtistController {

  private final ArtistService artistService;

  @Autowired
  public ArtistController(ArtistService artistService) {
    this.artistService = artistService;
  }

  @GetMapping
  public ResponseEntity getAllArtists() {
    List<Artist> artists = artistService.findAll();
    return ResponseEntity.ok().body(new ArtistGetAllResponse(artists));
  }

  @GetMapping(value = "/{id}")
  public ResponseEntity getArtistById(@PathVariable Long id) {
    Optional<Artist> adoption = artistService.findById(id);
    if (adoption.isPresent()) {
      ResponseEntity<ArtistEntityResponse> resEnt = new ResponseEntity<>(
          new ArtistEntityResponse(adoption.get()),
          HttpStatus.OK);
      return resEnt;
    } else {
      return ResponseEntity.notFound().build();
    }
  }

  @PostMapping(value = "/create", produces = MediaType.APPLICATION_JSON_VALUE)
  public ResponseEntity createArtist(@RequestBody ArtistCreateDto dto) {
    System.out.println("HERE HERE HERE ");
    Artist artist = new Artist(dto.name());
    Artist artistSaved = artistService.save(artist);

    URI uri = HttpHelper.uri(artistSaved.getId());
    return ResponseEntity.created(uri).body(new ArtistEntityResponse(artistSaved));
  }

  @PutMapping(value = "/{id}")
  public ResponseEntity updateArtist(@PathVariable Long id, @RequestBody ArtistEntityDto dto) {
    Optional<Artist> loadedArtist = artistService.findById(id);
    if (!loadedArtist.isPresent()){
      return ResponseEntity.notFound().build();
    } else {
      Artist updatedArtist = artistService.update(id, dto);
      return ResponseEntity.ok().body(new ArtistEntityResponse(updatedArtist));
    }
  }

  @DeleteMapping(value = "/{id}")
  public ResponseEntity deleteArtist(@PathVariable Long id) {
    artistService.deleteById(id);
    return ResponseEntity.ok().body(new ArtistDeleteResponse(id));
  }
}
