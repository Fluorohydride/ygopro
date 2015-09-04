--ロード・ウォリアー
function c2322421.initial_effect(c)
	--synchro summon
	aux.AddSynchroProcedure(c,c2322421.tfilter,aux.NonTuner(nil),2)
	c:EnableReviveLimit()
	--spsummon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(2322421,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetCountLimit(1)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTarget(c2322421.tg)
	e1:SetOperation(c2322421.op)
	c:RegisterEffect(e1)
end
function c2322421.tfilter(c)
	return c:IsCode(71971554) or c:IsHasEffect(20932152)
end
function c2322421.filter(c,e,tp)
	return c:IsLevelBelow(2) and c:IsRace(RACE_WARRIOR+RACE_MACHINE) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c2322421.tg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c2322421.filter,tp,LOCATION_DECK,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_DECK)
end
function c2322421.op(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c2322421.filter,tp,LOCATION_DECK,0,1,1,nil,e,tp)
	if g:GetCount()>0 then
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
	end
end
