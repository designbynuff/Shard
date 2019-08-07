# shard
Interactive Installation for Ontario Place Winter Lights Festival 2017-18

# "PARTY MODE" FOR NIGHT VISION @ MOCA 2019
- because there will be loud music, we probably won't be able to pick up user-generated audio, so it makes sense to have the IR sensors be interactive and the microphones respond to the music or inactive.
- IR sensors can detect distance to nearest object, we could do something like changing colour as people get closer/further
- Let's say for example we take all audio above a certain threshold and map that to brightness of the white lights. Then we map distance to hue of the RGB lights
- I'd like to have this be a real-time response this time, as opposed to the slight delay we put on OP Shard. Wondering if the Arduino can handle live sampling/behaviours, will have to test
- We won't know audio levels until the day of during sound check, so best guess for now
