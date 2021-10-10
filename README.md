
# Robotiklubi projekt ICU

ICU is a folkrace robot that uses visual input to race.

## Installation

...
## Usage

```python
...
```

## Development log

### 03.01.2020
Developing a driving system based on **histograms**...
Base image: 
![Default footage](https://gitlab.com/Fyoxy/icu/-/raw/master/readme/Footage.gif)
 - Firstly I change the colorspace to HSV so I could get the floor that the robot will be driving on as white and walls / obstacles as black
  ```python
def threshold(img):
	hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
	lowerFloor = np.array([8,10,39])
	upperFloor = np.array([52,102,238])
	maskFloor = cv2.inRange(hsv, lowerFloor, upperFloor)
	
	return maskFloor
```
 - Secondly I warp the image to hide some of the noise and to get a better view of the floor and the walls
```python
def warpImg(img, points, w, h): # w, h being the width and height of the input image 
    pt1 = np.float32(points)
    pt2 = np.float32([[0, 0], [w, 0], [0, h], [w, h]])
    matrix = cv2.getPerspectiveTransform(pt1, pt2) # calculates the points where I can warp the image

    imgWarp = cv2.warpPerspective(img, matrix, (w, h)) # warps the image

    return imgWarp
```
![Warped HSV footage](https://gitlab.com/Fyoxy/icu/-/raw/master/readme/WarpedHSV.gif)
 - Then I take the values from the black and white histogram. Depending on which side has more white pixels the steering angle changes with the curvature of the lane. 
 The center point is marked with the dot at the bottom of the screen which marks the center position.
```python
# Calculations to get the center position of the histogram
maxValue = np.max(histogramValues)
minValue = minPer * maxValue

indexArray = np.where(histogramValues >= minValue)
basePoint = int(np.average(indexArray))
```
![Histogram footage](https://gitlab.com/Fyoxy/icu/-/raw/master/readme/Histogram.gif)

## Contributing
...
## License
...
