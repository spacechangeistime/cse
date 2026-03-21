package main

import (
	"log"
	"net/http"

	"github.com/gin-gonic/gin"
)

type Album struct {
	ID     string  `json:ID`
	Title  string  `json:Title`
	Artist string  `json:Artist`
	Price  float64 `json:Price`
}

var Albums = []Album{
	{ID: "1", Title: "first", Artist: "Artist1", Price: 19.0},
	{ID: "2", Title: "second", Artist: "Artist2", Price: 50.0},
	{ID: "3", Title: "third", Artist: "Artist3", Price: 30.0},
}

func getAlbums(c *gin.Context) {
	c.IndentedJSON(http.StatusOK, Albums)
}
func getAlbumById(c *gin.Context) {
	id := c.Param("id")
	// find the album whose id matches the id requested by the user
	for _, album := range Albums {
		if album.ID == id {
			c.IndentedJSON(http.StatusOK, album)
			return
		}
	}
	// album with requested id doesn't exist
	c.IndentedJSON(http.StatusNotFound, gin.H{"message": "album not found!"})
}
func postAlbum(c *gin.Context) {
	var newAlbum Album
	// BindJSON binds the recieved json to newAlbum
	if err := c.BindJSON(&newAlbum); err != nil {
		log.Println("error: %v", err)
	}
	Albums = append(Albums, newAlbum)
	c.IndentedJSON(http.StatusCreated, newAlbum)
}

func main() {
	router := gin.Default()
	router.GET("/", getAlbums)
	router.GET("/albums", getAlbums)
	router.GET("/albums/:id", getAlbumById)
	router.POST("/albums", postAlbum)

	router.Run("localhost:8080")
}
