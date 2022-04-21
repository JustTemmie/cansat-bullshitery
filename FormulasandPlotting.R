setwd("C://Users/i1aar/OneDrive/Dokumenter/vg1/ToF/CanSat/NatCatSat")

csdata0 <- read.delim("C://Users/i1aar/OneDrive/Dokumenter/vg1/ToF/CanSat/NatCatSat/the actual data/dropdatasorted.TXT")
csdata <- read.delim("C://Users/i1aar/OneDrive/Dokumenter/vg1/ToF/CanSat/NatCatSat/the actual data/dropdatasorted2.TXT")
latdata <- read.delim("C://Users/i1aar/OneDrive/Dokumenter/vg1/ToF/CanSat/NatCatSat/the actual data/latencydata.TXT")

#ggplot libs
library(ggplot2)
library(ggpubr)

a <- 3.354016*10**-3
b <- 2.569850*10**-4
c <- 2.620131*10**-6
d <- 6.383091*10**-8
thing <- log(csdata$NTCTemp/(5-csdata$NTCTemp)+1)
csdata$NTCTemp2 <- (a + b*thing + c*thing**2 + d*thing**3)**-1 -272.15

# ALTITUDE
#initial temp K
t1 = 272.15 + 11
#temp gradient
tg = -0.0065
#pressure
p = csdata$MPXPressure2
#initial pressure (MPX ground lvl)
p1 = 1207.693
#gas constant thing
R = 287.06
#gravity (:
g = 9.81
#initial alt
h1 = 37
#formula
csdata$CalculatedAltitude <- (t1/tg)*( (csdata$MPXPressure2/p1)**((-tg*R)/g) - 1) + h1

# LM: bit -> V and * 100
csdata$LMTemp2 <- csdata$LMTemp / 1024 * 500

# MPX4115: bit -> V, then you do all these operations just because E said so start of dec also might say so in CanSat book or something. Probably
csdata$MPXPressure2 <- 10*((5*csdata$MPXPressure/1024)/(0.009*5)+(0.095/0.009))

# BMP pressure: just divide by 100 cause hPa - or not??
csdata$BMPPressure2 <- csdata$BMPPressure

csdata$UpTime <- (csdata$Uptime - 3953617) / 1000
csdata0$UpTime <- (csdata0$Uptime - 3953617) / 1000

latdata$TDBsecs <- (latdata$TDBmillis - 3849964) / 1000
latdata$TFBsecs <- (latdata$TFBmillis - 3954096) / 1000


# plots
gpMPX <- ggplot(csdata)+
  geom_line(aes(UpTime, MPXPressure2))+
  labs(x = "seconds", y = "hPa", title = "MPX4115", subtitle = "Atmospheric pressure")+
  theme_minimal()

gpBMPpres <- ggplot(csdata)+
  geom_line(aes(UpTime, BMPPressure2))+
  labs(x = "seconds", y = "hPa", title = "BMP280", subtitle = "Atmospheric pressure")+
  theme_minimal()

gppres <- ggarrange(gpMPX, gpBMPpres, nrow = 1)

gpBMPalt <- ggplot(csdata)+
  geom_line(aes(UpTime, BMPCalculatedAltitude))+
  labs(x = "seconds", y = "meters above sea level", title = "BMP280", subtitle = "Altitude")+
  theme_minimal()

gpCalcAlt <- ggplot(csdata)+
  geom_line(aes(UpTime, CalculatedAltitude))+
  labs(x = "seconds", y = "meters above sea level", title = "Altitude")+
  theme_minimal()

gpLM <- ggplot(csdata)+
  geom_line(aes(UpTime, LMTemp2))+
  labs(x = "seconds", y = "°C", title = "LM35")+
  theme_minimal()

gpNTC <- ggplot(csdata)+
  geom_line(aes(UpTime, NTCTemp2))+
  labs(x = "seconds", y = "°C", title = "NTCLE100E3103JB0")+
  theme_minimal()

gpBMPtemp <- ggplot(csdata)+
  geom_line(aes(UpTime, BMPTemp))+
  labs(x = "seconds", y = "°C", title = "BMP280")+
  theme_minimal()

#pres
gppres2 <- ggplot(csdata, aes(x = UpTime))+
  geom_line(aes(y = MPXPressure2, color = "MPX4115"))+
  geom_line(aes(y = BMPPressure2, color = "BMP280"))+
  labs(x = "seconds", y = "hPa", title = "Atmospheric pressure")+
  theme_minimal()

#temp
gpBMPtemp0 <- ggplot(csdata0)+
  geom_line(aes(UpTime, BMPTemp))+
  labs(x = "seconds", y = "°C", title = "BMP280")+
  theme_minimal()

gptemp0 <- ggarrange(gpBMPtemp0, gpNTC, gpLM, nrow = 3)

#gptemp1 <- ggarrange(gpBMPtemp, gpNTC, nrow = 2)

# LATENCY

gplattot <- ggplot(latdata)+
  geom_line(aes(TDBsecs, latencytotal))+
  labs(x = "seconds", y = "duration of cycle (seconds)", title = "Total latency")+
  theme_minimal()

latdata$difDBFB <- (latdata$TFBmillis - latdata$TDBmillis) / 1000

gpdifDBFB <- ggplot(latdata)+
  geom_line(aes(TDBsecs, difDBFB))+
  labs(x = "seconds", y = "difference TDB & TFB (seconds)", title = "Latency TDB -> TFB")+
  theme_minimal()


