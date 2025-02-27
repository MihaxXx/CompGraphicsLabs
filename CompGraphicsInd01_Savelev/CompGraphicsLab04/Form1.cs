﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CompGraphicsInd01_Savelev
{
    public partial class Form1 : Form
    {
        /// <summary>
        /// Список всех точек на pictureBox
        /// </summary>
        private LinkedList<PointF> points = new LinkedList<PointF>();

        /// <summary>
        /// Выпуклый полигон
        /// </summary>
        private LinkedList<PointF> polygon = new LinkedList<PointF>();

        private Pen pen = new Pen(Color.Black);
        private Bitmap bmp;

        public Form1()
        {
            InitializeComponent();

            bmp = new Bitmap(pictureBox1.Width, pictureBox1.Height);
            pictureBox1.Image = bmp;
        }

        private void pictureBox1_MouseClick(object sender, MouseEventArgs e)
        {
            AddPoint(e.Location);
            DrawPrimitives();
        }

        /// <summary>
        /// Добвыляет точку <paramref name="point"/> на pictureBox и в полигон, если требуется
        /// </summary>
        /// <param name="point"></param>
        private void AddPoint(PointF point)
        {
            points.AddLast(point);
            if (points.Count == 3)
            {
                polygon = new LinkedList<PointF>(points.OrderBy(p => -p.X).ThenBy(p => p.Y));
            }
            else if (points.Count > 3)
            {
                if (!IsPointInside(polygon, point))
                    AlterPolygon(point);
            }
        }

        /// <summary>
        /// Добавляет точку <paramref name="pointToAdd"/> в полигон <c>polygon</c>.
        /// </summary>
        /// <param name="pointToAdd">Точка для добавления.</param>
        private void AlterPolygon(PointF pointToAdd)
        {
            var left = polygon.First(p => polygon.All(point => PointFPosition(new Tuple<PointF, PointF>(pointToAdd, p), point) != Position.Left));
            var right = polygon.First(p => polygon.All(point => PointFPosition(new Tuple<PointF, PointF>(pointToAdd, p), point) != Position.Right));

            if (DistanceBetweenPoints(pointToAdd, polygon.Find(right).Value) > DistanceToSegment(pointToAdd, (left, right).ToTuple(), out PointF t))
            {
                var rNode = polygon.Find(right);
                var lNode = (polygon.Find(left) == polygon.First) ? polygon.Last : polygon.Find(left).Previous;
                while (rNode != lNode)
                {
                    var nextlNode = (lNode == polygon.First) ? polygon.Last : lNode.Previous;
                    polygon.Remove(lNode);
                    lNode = nextlNode;
                }
                polygon.AddBefore(polygon.Find(left), pointToAdd);
            }
            else
            {
                var rNode = (polygon.Find(right) == polygon.Last) ? polygon.First : polygon.Find(right).Next;
                var lNode = polygon.Find(left);
                while (rNode != lNode)
                {
                    var nextrNode = (rNode == polygon.Last) ? polygon.First : rNode.Next;
                    polygon.Remove(rNode);
                    rNode = nextrNode;
                }
                polygon.AddAfter(polygon.Find(right), pointToAdd);
            }
        }

        enum Position { Left, Right, Undefined }
        /// <summary>
        /// Определяет положение точки относительно направленного ребра
        /// </summary>
        private Position PointFPosition(Tuple<PointF, PointF> edge, PointF b)
        {
            PointF O = edge.Item1;
            PointF a = edge.Item2;
            var sign = (b.X - O.X) * (a.Y - O.Y) - (b.Y - O.Y) * (a.X - O.X);

            if (sign > 0)
                return Position.Left;
            if (sign < 0)
                return Position.Right;
            return Position.Undefined;
        }

        /// <summary>
        /// Calculate the distance between points <paramref name="p1"/> and <paramref name="p2"/>.
        /// </summary>
        /// <param name="p1"></param>
        /// <param name="p2"></param>
        /// <returns>The distance.</returns>
        double DistanceBetweenPoints(PointF p1, PointF p2)
        {
            return Math.Sqrt(Math.Pow(p2.X - p1.X, 2) + Math.Pow(p2.Y - p1.Y, 2) * 1.0);
        }

        /// <summary>
        /// Calculate the distance between point <paramref name="pt"/> and the <paramref name="segment"/>.
        /// </summary>
        /// <param name="pt">Point.</param>
        /// <param name="segment">Segment</param>
        /// <param name="closest">Closest to <paramref name="pt"/> point lying on <paramref name="segment"/>.</param>
        /// <returns>The distance.</returns>
        private double DistanceToSegment(PointF pt, Tuple<PointF,PointF> segment, out PointF closest)
        {
            (var p1, var p2) = (segment.Item1, segment.Item2);

            float dx = p2.X - p1.X;
            float dy = p2.Y - p1.Y;
            if ((dx == 0) && (dy == 0))
            {
                // It's a point not a line segment.
                closest = p1;
                dx = pt.X - p1.X;
                dy = pt.Y - p1.Y;
                return Math.Sqrt(dx * dx + dy * dy);
            }

            // Calculate the t that minimizes the distance.
            float t = ((pt.X - p1.X) * dx + (pt.Y - p1.Y) * dy) /
                (dx * dx + dy * dy);

            // See if this represents one of the segment's
            // end points or a point in the middle.
            if (t < 0)
            {
                closest = new PointF(p1.X, p1.Y);
                dx = pt.X - p1.X;
                dy = pt.Y - p1.Y;
            }
            else if (t > 1)
            {
                closest = new PointF(p2.X, p2.Y);
                dx = pt.X - p2.X;
                dy = pt.Y - p2.Y;
            }
            else
            {
                closest = new PointF(p1.X + t * dx, p1.Y + t * dy);
                dx = pt.X - closest.X;
                dy = pt.Y - closest.Y;
            }

            return Math.Sqrt(dx * dx + dy * dy);
        }

        /// <summary>
        /// Определяет, находится ли точка <paramref name="point"/> внутри выпуклого полигона <paramref name="polygon"/>.
        /// </summary>
        /// <param name="polygon">Выпуклый полигон.</param>
        /// <param name="point">Точка.</param>
        /// <returns>Принадлежность.</returns>
        private bool IsPointInside(LinkedList<PointF> polygon, PointF point)
        {
            Position lastPosition = Position.Undefined;
            bool isFirst = true;
            var list = polygon;
            var cur = list.First;
            var first_val = cur.Value;
            Tuple<PointF, PointF> edge;
            Position pos;
            do  //while (cur != )
            {
                edge = Tuple.Create(cur.Value, cur.Next.Value);
                pos = PointFPosition(edge, point);
                if (isFirst)
                {
                    lastPosition = pos;
                    isFirst = false;
                }
                else
                {
                    if (pos != lastPosition)
                        return false;
                }
                cur = cur.Next;
            } while (cur != list.Last);

            edge = Tuple.Create(cur.Value, first_val);
            pos = PointFPosition(edge, point);
            if (pos != lastPosition)
                return false;
            return true;
        }

        /// <summary>
        /// Обновление pictureBox
        /// </summary>
        private void DrawPrimitives()
        {
            bmp = new Bitmap(pictureBox1.Width, pictureBox1.Height);
            Graphics g = Graphics.FromImage(bmp);
            foreach (var point in points) // рисуем точки (9 пикселей: от -1 до +1 от точки)
            {
                for (int i = -1; i < 2; i++)
                    for (int j = -1; j < 2; j++)
                    {
                        var (x, y) = ((int)point.X + i, (int)point.Y + j);
                        if(x>=0 && y>=0)
                            bmp.SetPixel(x, y, Color.Black);
                    }
            }
            if(polygon != null)// рисуем полигон
            {
                var cur = polygon.First;
                if (cur != polygon.Last)
                    g.DrawLine(pen, polygon.First.Value, polygon.Last.Value);
                while (cur != polygon.Last)
                {
                    g.DrawLine(pen, cur.Value, cur.Next.Value);
                    cur = cur.Next;
                }
            }
            foreach (var point in polygon) // рисуем точки, включённые в полигон, красным
            {
                for (int i = -1; i < 2; i++)
                    for (int j = -1; j < 2; j++)
                    {
                        var (x, y) = ((int)point.X + i, (int)point.Y + j);
                        if (x >= 0 && y >= 0)
                            bmp.SetPixel(x, y, Color.Red);
                    }
            }

            pictureBox1.Image = bmp;
        }


        // Кнопка "Очистить"
        private void Clear_Click(object sender, EventArgs e)
        {
            bmp = new Bitmap(pictureBox1.Width, pictureBox1.Height);
            pictureBox1.Image = bmp;
            points.Clear();
            polygon.Clear();

            pictureBox1.Enabled = true;
        }

        //перемножение матриц
        private double[,] multMatrix(double[,] m1, double[,] m2)
        {
            double[,] res = new double[m1.GetLength(0), m2.GetLength(1)];

            for (int i = 0; i < m1.GetLength(0); ++i)
                for (int j = 0; j < m2.GetLength(1); ++j)
                    for (int k = 0; k < m2.GetLength(0); k++)
                    {
                        res[i, j] += m1[i, k] * m2[k, j];
                    }

            return res;
        }
    }
}
