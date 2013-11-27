--オーバーリミット
function c23282832.initial_effect(c)
	--activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_END_PHASE)
	e1:SetCost(c23282832.cost)
	e1:SetTarget(c23282832.tg)
	e1:SetOperation(c23282832.op)
	c:RegisterEffect(e1)
end
function c23282832.filter(c,e,tp,tid)
	return c:GetTurnID()==tid and c:IsReason(REASON_BATTLE) and c:IsType(TYPE_NORMAL)
		and c:IsAttackBelow(1000) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c23282832.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckLPCost(tp,500) end
	Duel.PayLPCost(tp,500)
end
function c23282832.tg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c23282832.filter,tp,LOCATION_GRAVE,LOCATION_GRAVE,1,nil,e,tp,Duel.GetTurnCount()) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,PLAYER_ALL,LOCATION_GRAVE)
end
function c23282832.op(e,tp,eg,ep,ev,re,r,rp)
	local ft1=Duel.GetLocationCount(tp,LOCATION_MZONE)
	if ft1<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c23282832.filter,tp,LOCATION_GRAVE,LOCATION_GRAVE,ft1,ft1,nil,e,tp,Duel.GetTurnCount())
	if g:GetCount()>0 then
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
	end
end
