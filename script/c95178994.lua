--ジャイアントウィルス
function c95178994.initial_effect(c)
	--battle destroyed
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(95178994,0))
	e1:SetCategory(CATEGORY_DAMAGE+CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_BATTLE_DESTROYED)
	e1:SetCondition(c95178994.condition)
	e1:SetTarget(c95178994.target)
	e1:SetOperation(c95178994.operation)
	c:RegisterEffect(e1)
end
function c95178994.condition(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():IsLocation(LOCATION_GRAVE)
		and e:GetHandler():IsReason(REASON_BATTLE)
end
function c95178994.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,500)
end
function c95178994.filter(c,e,tp)
	return c:IsCode(95178994) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c95178994.operation(e,tp,eg,ep,ev,re,r,rp)
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	Duel.Damage(1-tp,500,REASON_EFFECT)
	if ft<=0 then return end
	local g=Duel.GetMatchingGroup(c95178994.filter,tp,LOCATION_DECK,0,nil,e,tp)
	if g:GetCount()>0 and Duel.SelectYesNo(tp,aux.Stringid(95178994,1)) then
		Duel.BreakEffect()
		Duel.SpecialSummonStep(g:GetFirst(),0,tp,tp,false,false,POS_FACEUP_ATTACK)
		if ft>1 and g:GetCount()>1 and Duel.SelectYesNo(tp,aux.Stringid(95178994,1)) then
			Duel.SpecialSummonStep(g:GetNext(),0,tp,tp,false,false,POS_FACEUP_ATTACK)
		end
		Duel.SpecialSummonComplete()
	end
end
