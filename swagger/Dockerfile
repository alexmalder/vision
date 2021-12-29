FROM quay.io/goswagger/swagger

WORKDIR /app

COPY . .

EXPOSE 5000

CMD ["swagger", "serve", "swagger.yml", "--host", "0.0.0.0", "--port", "5000"]
