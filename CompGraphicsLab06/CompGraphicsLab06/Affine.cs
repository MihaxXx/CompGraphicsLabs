﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CompGraphicsLab06
{
    class Affine
    {
        static private float[,] matrixColumnFromPoint3D(Point3D point)
        {
            return new float[,] { { point.X }, { point.Y }, { point.Z }, { 1 } };

        }

        /// <summary>
        /// Применения матрицы преобразований к каждой точке многогранника
        /// </summary>
        static private void ChangePolyhedron(Polyhedron polyhedron, float[,] matrix)
        {
            List<Point3D> points = new List<Point3D>();
            for (int i = 0; i < polyhedron.Vertexes.Count; ++i) // применяем преобразования к каждой точке
            {
                var matrixPoint = Projection.MultMatrix(matrix, matrixColumnFromPoint3D(polyhedron.Vertexes[i]));
                Point3D newPoint = new Point3D(matrixPoint[0, 0] / matrixPoint[3, 0], matrixPoint[1, 0] / matrixPoint[3, 0], matrixPoint[2, 0] / matrixPoint[3, 0]);
                polyhedron.Vertexes[i] = newPoint;
            }
        }
        /// <summary>
        /// Сдвинуть многогранник
        /// </summary>
        static public void translate(Polyhedron polyhedron, float tx, float ty, float tz)
        {
            float[,] translation = { { 1, 0, 0, tx },
                                     { 0, 1, 0, ty },
                                     { 0, 0, 1, tz },
                                     { 0, 0, 0,  1 }};

            ChangePolyhedron(polyhedron, translation);
        }

        /// <summary>
        /// Масштабирование относительно оси
        /// </summary>
        static public void scale(Polyhedron polyhedron, float mx, float my, float mz)
        {
            float[,] scale = { { mx,  0,  0,  0 },
                               {  0, my,  0,  0 },
                               {  0,  0, mz,  0 },
                               {  0,  0,  0,  1 }};

            ChangePolyhedron(polyhedron, scale);
        }

        /// <summary>
        /// Поворот
        /// </summary>
        /// <param name="polyhedron"></param>
        /// <param name="angleX">угол поворота по OX в градусах</param>
        /// <param name="angleY">угол поворота по OY в градусах</param>
        /// <param name="angleZ">угол поворота по OZ в градусах</param>
        static public void rotation(Polyhedron polyhedron, float angleX, float angleY, float angleZ)
        {
            Point3D shiftPoint = polyhedron.Center();
            float shiftX = shiftPoint.X,
                  shiftY = shiftPoint.Y,
                  shiftZ = shiftPoint.Z;

            translate(polyhedron, -shiftX, -shiftY, -shiftZ);

            float sin = (float)Math.Sin(angleX * Math.PI / 180);
            float cos = (float)Math.Cos(angleX * Math.PI / 180);
            float[,] matrixX = { { 1,  0,   0,  0},
                                 { 0, cos,-sin, 0},
                                 { 0, sin, cos, 0},
                                 { 0,  0,   0,  1}};

            sin = (float)Math.Sin(angleY * Math.PI / 180);
            cos = (float)Math.Cos(angleY * Math.PI / 180);
            float[,] matrixY = { { cos, 0, sin, 0},
                                 {  0,  1,  0,  0},
                                 {-sin, 0, cos, 0},
                                 {  0,  0,  0,  1}};

            sin = (float)Math.Sin(angleZ * Math.PI / 180);
            cos = (float)Math.Cos(angleZ * Math.PI / 180);
            float[,] matrixZ = { { cos, -sin, 0, 0},
                                 { sin,  cos, 0, 0},
                                 {  0,    0,  1, 0},
                                 {  0,    0,  0, 1}};

            ChangePolyhedron(polyhedron, Projection.MultMatrix(Projection.MultMatrix(matrixX, matrixY), matrixZ));

            translate(polyhedron, shiftX, shiftY, shiftZ);
        }

        // Отражение относительно выбранной координатной плоскости
        public static void reflection(Polyhedron polyhedron, string plane)
        {
            float[,] matrix;
            switch (plane)
            {
                case "xy":
                    matrix = new float[,] {{ 1, 0,  0, 0 },
                                           { 0, 1,  0, 0 },
                                           { 0, 0, -1, 0 },
                                           { 0, 0,  0, 1 }};
                    break;
                case "xz":
                    matrix = new float[,] {{ 1,  0, 0, 0 },
                                           { 0, -1, 0, 0 },
                                           { 0,  0, 1, 0 },
                                           { 0,  0, 0, 1 }};
                    break;
                case "yz":
                    matrix = new float[,] {{ -1, 0, 0, 0 },
                                           {  0, 1, 0, 0 },
                                           {  0, 0, 1, 0 },
                                           {  0, 0, 0, 1 }};
                    break;
                default:
                    matrix = new float[,] {{ 1, 0, 0, 0 },
                                           { 0, 1, 0, 0 },
                                           { 0, 0, 1, 0 },
                                           { 0, 0, 0, 1 }};
                    break;
            }
            ChangePolyhedron(polyhedron, matrix);
        }

        // Масштабирование относительно центра
        public static void scaleCenter(Polyhedron polyhedron, float a)
        {
            Point3D shiftPoint = polyhedron.Center();
            float shiftX = shiftPoint.X,
                  shiftY = shiftPoint.Y,
                  shiftZ = shiftPoint.Z;

            translate(polyhedron, -shiftX, -shiftY, -shiftZ);

            float[,] scale = { { a, 0, 0, 0 },
                               { 0, a, 0, 0 },
                               { 0, 0, a, 0 },
                               { 0, 0, 0, 1 }};

            ChangePolyhedron(polyhedron, scale);
            translate(polyhedron, shiftX, shiftY, shiftZ);
        }

        /// <summary>
        /// Вращение многогранника вокруг прямой, проходящей через центр, параллельно выбранно координатной оси
        /// </summary>
        /// <param name="polyhedron"></param>
        /// <param name="angleX"></param>
        /// <param name="angleY"></param>
        /// <param name="angleZ"></param>
        public static void rotateCenter(Polyhedron polyhedron, float angleX, float angleY, float angleZ)
        {
            var center = polyhedron.Center();
            translate(polyhedron, -center.X, -center.Y, -center.Z);
            rotation(polyhedron, angleX, angleY, angleZ);
            translate(polyhedron, center.X, center.Y, center.Z);
        }

        public static void rotateAboutLine(Polyhedron polyhedron, float angle, Edge line)
        {
            var vect = line.To - line.From;
            var len = Math.Sqrt(Math.Pow(vect.X, 2) + Math.Pow(vect.Y, 2) + Math.Pow(vect.Z, 2));
            var (l, m, n) = ((float)(vect.X / len), (float)(vect.Y / len), (float)(vect.Z / len));

            float sin = (float)Math.Sin(angle * Math.PI / 180);
            float cos = (float)Math.Cos(angle * Math.PI / 180);

            /* float[,] matr = {
                 { l*l+cos*(1-l*l), l*(1-cos)*m+n*sin, l*(1-cos)*n-m*sin, 0 },
                 { l*(1-cos)*m-n*sin, m*m+cos*(1-m*m), m*(1-cos)*n+l*sin, 0 },
                 { l*(1-cos)*n+m*sin, m*(1-cos)*n-l*sin, n*n+cos*(1-n*n), 0 },
                 { 0,                    0,              0,               1 }
             };*/

            float[,] matr = { { l*l+cos*(1-l*l),   l*(1-cos)*m-n*sin, l*(1-cos)*n+m*sin, 0 },
                              { l*(1-cos)*m+n*sin, m*m+cos*(1-m*m),   m*(1-cos)*n-l*sin, 0 },
                              { l*(1-cos)*n-m*sin, m*(1-cos)*n+l*sin, n*n+cos*(1-n*n),   0 },
                              { 0,                 0,                 0,                 1 }};

            ChangePolyhedron(polyhedron, matr);
        }
    }
}
