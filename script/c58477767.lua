--女王の選択
function c58477767.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_BATTLE_DESTROYED)
	e1:SetCondition(c58477767.condition)
	e1:SetTarget(c58477767.target)
	e1:SetOperation(c58477767.operation)
	c:RegisterEffect(e1)
end
function c58477767.check(c1,c2,tp)
	return c1:IsLocation(LOCATION_GRAVE) and c1:IsReason(REASON_BATTLE) and c1:GetPreviousControler()~=tp and c2:IsSetCard(0x4)
end
function c58477767.condition(e,tp,eg,ep,ev,re,r,rp)
	local dc=eg:GetFirst()
	local bc=dc:GetBattleTarget()
	return c58477767.check(dc,bc,tp) or c58477767.check(bc,dc,tp)
end
function c58477767.spfilter(c,e,tp)
	return c:IsLevelBelow(4) and c:IsSetCard(0x4) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c58477767.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c58477767.spfilter,tp,LOCATION_DECK,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_DECK)
end
function c58477767.operation(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c58477767.spfilter,tp,LOCATION_DECK,0,1,1,nil,e,tp)
	if g:GetCount()>0 then
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
	end
end
