add_executable(buneslots 
  main.c 
)

target_link_libraries(buneslots 
  SDL3::SDL3
  SDL3_image::SDL3_image
)