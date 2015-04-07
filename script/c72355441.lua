--エクシーズ・ギフト
function c72355441.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DRAW)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c72355441.condition)
	e1:SetTarget(c72355441.target)
	e1:SetOperation(c72355441.activate)
	c:RegisterEffect(e1)
end
function c72355441.cfilter(c)
	return c:IsFaceup() and c:IsType(TYPE_XYZ)
end
function c72355441.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c72355441.cfilter,tp,LOCATION_MZONE,0,2,nil)
end
function c72355441.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsPlayerCanDraw(tp,2) and Duel.CheckRemoveOverlayCard(tp,1,0,2,REASON_EFFECT) end
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,2)
end
function c72355441.activate(e,tp,eg,ep,ev,re,r,rp)
	if not Duel.CheckRemoveOverlayCard(tp,1,0,2,REASON_EFFECT) then return end
	Duel.RemoveOverlayCard(tp,1,0,2,2,REASON_EFFECT)
	Duel.Draw(tp,2,REASON_EFFECT)
end
