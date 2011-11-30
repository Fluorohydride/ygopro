--現世と冥界の逆転
function c17484499.initial_effect(c)
	--Activate(summon)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_DRAW_PHASE)
	e1:SetCondition(c17484499.condition)
	e1:SetCost(c17484499.cost)
	e1:SetOperation(c17484499.activate)
	c:RegisterEffect(e1)
end
function c17484499.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetFieldGroupCount(tp,LOCATION_GRAVE,0)>=15
end
function c17484499.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckLPCost(tp,1000)
	else Duel.PayLPCost(tp,1000) end
end
function c17484499.activate(e,tp,eg,ep,ev,re,r,rp)
	Duel.SwapDeckAndGrave(tp)
	Duel.SwapDeckAndGrave(1-tp)
end
