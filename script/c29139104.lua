--荒ぶるアウス
function c29139104.initial_effect(c)
	--spsummon
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetCost(c29139104.spcost)
	e1:SetTarget(c29139104.sptg)
	e1:SetOperation(c29139104.spop)
	c:RegisterEffect(e1)
end
function c29139104.spcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckReleaseGroup(tp,Card.IsAttribute,1,e:GetHandler(),ATTRIBUTE_EARTH) end
	local g=Duel.SelectReleaseGroup(tp,Card.IsAttribute,1,1,e:GetHandler(),ATTRIBUTE_EARTH)
	Duel.Release(g,REASON_COST)
end
function c29139104.filter(c,e,tp)
	return c:IsAttribute(ATTRIBUTE_EARTH) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c29139104.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c29139104.filter,tp,LOCATION_HAND,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_HAND)
end
function c29139104.spop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c29139104.filter,tp,LOCATION_HAND,0,1,1,nil,e,tp)
	if g:GetCount()>0 then
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		e1:SetCode(EVENT_LEAVE_FIELD)
		e1:SetRange(LOCATION_MZONE)
		e1:SetOperation(c29139104.desop)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		g:GetFirst():RegisterEffect(e1)
	end
end
function c29139104.desop(e,tp,eg,ep,ev,re,r,rp)
	if eg:IsExists(Card.IsCode,1,nil,29139104) then
		Duel.Destroy(e:GetHandler(),REASON_EFFECT)
	end
end
