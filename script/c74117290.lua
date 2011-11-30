--暗黒界の取引
function c74117290.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DRAW)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c74117290.target)
	e1:SetOperation(c74117290.activate)
	c:RegisterEffect(e1)
end
function c74117290.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		local d1=Duel.GetFieldGroupCount(tp,LOCATION_DECK,0)
		local d2=Duel.GetFieldGroupCount(tp,0,LOCATION_DECK)
		return d1>0 and d2>0
	end
	Duel.SetOperationInfo(0,CATEGORY_HANDES,nil,0,PLAYER_ALL,1)
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,PLAYER_ALL,1)
end
function c74117290.activate(e,tp,eg,ep,ev,re,r,rp)
	Duel.Draw(tp,1,REASON_EFFECT)
	Duel.Draw(1-tp,1,REASON_EFFECT)
	Duel.BreakEffect()
	Duel.DiscardHand(tp,aux.TRUE,1,1,REASON_EFFECT+REASON_DISCARD)
	Duel.DiscardHand(1-tp,aux.TRUE,1,1,REASON_EFFECT+REASON_DISCARD)
end
