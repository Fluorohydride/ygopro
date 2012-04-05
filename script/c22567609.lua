--素早いモモンガ
function c22567609.initial_effect(c)
	--battle destroyed
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(22567609,0))
	e1:SetCategory(CATEGORY_RECOVER+CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_BATTLE_DESTROYED)
	e1:SetCondition(c22567609.condition)
	e1:SetTarget(c22567609.target)
	e1:SetOperation(c22567609.operation)
	c:RegisterEffect(e1)
end
function c22567609.condition(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():IsLocation(LOCATION_GRAVE)
		and bit.band(e:GetHandler():GetReason(),REASON_BATTLE)~=0
end
function c22567609.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_RECOVER,nil,0,tp,1000)
end
function c22567609.filter(c,e,tp)
	return c:IsCode(22567609) and c:IsCanBeSpecialSummoned(e,0,tp,false,false,POS_FACEDOWN)
end
function c22567609.operation(e,tp,eg,ep,ev,re,r,rp)
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	Duel.Recover(tp,1000,REASON_EFFECT)
	if ft<=0 then return end
	local g=Duel.GetMatchingGroup(c22567609.filter,tp,LOCATION_DECK,0,nil,e,tp)
	if g:GetCount()>0 and Duel.SelectYesNo(tp,aux.Stringid(22567609,1)) then
		Duel.BreakEffect()
		Duel.SpecialSummonStep(g:GetFirst(),0,tp,tp,false,false,POS_FACEDOWN_DEFENCE)
		if ft>1 and g:GetCount()>1 and Duel.SelectYesNo(tp,aux.Stringid(22567609,1)) then
			Duel.SpecialSummonStep(g:GetNext(),0,tp,tp,false,false,POS_FACEDOWN_DEFENCE)
		end
		Duel.SpecialSummonComplete()
	end
end
