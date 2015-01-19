--レアル・ジェネクス・アクセラレーター
function c73783043.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(73783043,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetRange(LOCATION_MZONE)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_TO_HAND)
	e1:SetTarget(c73783043.sptg)
	e1:SetOperation(c73783043.spop)
	c:RegisterEffect(e1)
end
function c73783043.filter(c,e,tp)
	return c:IsSetCard(0x2) and c:IsControler(tp) and c:IsPreviousLocation(LOCATION_DECK) and not c:IsPublic()
		and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c73783043.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0 and eg:IsExists(c73783043.filter,1,nil,e,tp) end
	local g=eg:Filter(c73783043.filter,nil,e,tp)
	if g:GetCount()==1 then
		Duel.ConfirmCards(1-tp,g)
		Duel.ShuffleHand(tp)
		Duel.SetTargetCard(g)
	else
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		local sg=g:Select(tp,1,1,nil)
		Duel.ConfirmCards(1-tp,sg)
		Duel.ShuffleHand(tp)
		Duel.SetTargetCard(sg)
	end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_HAND)
end
function c73783043.spop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP)
	end
end
