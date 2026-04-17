import unittest


class MapVisualizationTests(unittest.TestCase):
    def test_optimal_preview_keeps_terminal_segment_after_last_intersection_turn(self) -> None:
        from app import DashboardState

        state = DashboardState()
        state.ingest_line("OPTIMAL_PATH=LRSL")

        view = state.to_view()

        self.assertEqual(
            view["polyline_opt"],
            [
                (0.0, 0.0),
                (0.0, -1.0),
                (-1.0, -1.0),
                (-1.0, -2.0),
                (-1.0, -3.0),
                (-2.0, -3.0),
            ],
        )

    def test_solve_preview_includes_final_forced_turn_segment(self) -> None:
        from app import DashboardState

        state = DashboardState()
        for line in (
            "SOLVE_FORCED_TURN=R exits=R",
            "SOLVE_NODE step=1/4 exits=LSR turn=L",
            "SOLVE_NODE step=2/4 exits=LR turn=R",
            "SOLVE_NODE step=3/4 exits=LS turn=S",
            "SOLVE_NODE step=4/4 exits=LSR turn=L",
            "SOLVE_FORCED_TURN=R exits=R",
        ):
            state.ingest_line(line)

        view = state.to_view()

        self.assertEqual(
            view["polyline_solve_geo"],
            [
                (0.0, 0.0),
                (0.0, -1.0),
                (1.0, -1.0),
                (1.0, -2.0),
                (2.0, -2.0),
                (3.0, -2.0),
                (3.0, -3.0),
                (4.0, -3.0),
            ],
        )


if __name__ == "__main__":
    unittest.main()
