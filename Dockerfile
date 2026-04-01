FROM emscripten/emsdk:3.1.74

WORKDIR /app

COPY . .

RUN mkdir -p web

CMD ["make", "cpp"]
