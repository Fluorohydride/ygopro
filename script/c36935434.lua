--バイロード・サクリファイス
function c36935434.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_BATTLE_DESTROYED)
	e1:SetCondition(c36935434.condition)
	e1:SetTarget(c36935434.target)
	e1:SetOperation(c36935434.operation)
	c:RegisterEffect(e1)
end
function c36935434.cfilter(c,tp)
	return c:GetPreviousControler()==tp
end
function c36935434.condition(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c36935434.cfilter,1,nil,tp)
end
function c36935434.filter(c,e,tp)
	return c:IsCode(64268668) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c36935434.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c36935434.filter,tp,LOCATION_HAND,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_HAND)
end
function c36935434.operation(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c36935434.filter,tp,LOCATION_HAND,0,1,1,nil,e,tp)
	if g:GetCount()>0 then
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
	end
end
