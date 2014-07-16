--銀河遠征
function c60222582.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCountLimit(1,60222582+EFFECT_COUNT_CODE_OATH)
	e1:SetCondition(c60222582.condition)
	e1:SetTarget(c60222582.target)
	e1:SetOperation(c60222582.activate)
	c:RegisterEffect(e1)
end
function c60222582.cfilter(c)
	return c:IsFaceup() and c:IsLevelAbove(5) and (c:IsSetCard(0x55) or c:IsSetCard(0x7b))
end
function c60222582.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c60222582.cfilter,tp,LOCATION_MZONE,0,1,nil)
end
function c60222582.spfilter(c,e,tp)
	return c:IsLevelAbove(5) and (c:IsSetCard(0x55) or c:IsSetCard(0x7b)) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c60222582.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c60222582.spfilter,tp,LOCATION_DECK,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_DECK)
end
function c60222582.activate(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c60222582.spfilter,tp,LOCATION_DECK,0,1,1,nil,e,tp)
	if g:GetCount()>0 then
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP_DEFENCE)
	end
end
