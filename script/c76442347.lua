--子型ペンギン
function c76442347.initial_effect(c)
	--flip effect
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_FLIP)
	e1:SetOperation(c76442347.flipop)
	c:RegisterEffect(e1)
	--special summon
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(76442347,0))
	e2:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetCode(EVENT_TO_GRAVE)
	e2:SetCondition(c76442347.spcon)
	e2:SetTarget(c76442347.sptg)
	e2:SetOperation(c76442347.spop)
	c:RegisterEffect(e2)
end
function c76442347.flipop(e,tp,eg,ep,ev,re,r,rp)
	e:GetHandler():RegisterFlagEffect(76442347,RESET_EVENT+0x57a0000,0,0)
end
function c76442347.spcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetFlagEffect(76442347)~=0
end
function c76442347.filter(c,e,tp)
	return c:IsSetCard(0x5a) and c:GetCode()~=76442347
		and (c:IsCanBeSpecialSummoned(e,0,tp,false,false)
		or c:IsCanBeSpecialSummoned(e,0,tp,false,false,POS_FACEDOWN))
end
function c76442347.sptg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c76442347.filter(chkc,e,tp) end
	if chk==0 then return true end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectTarget(tp,c76442347.filter,tp,LOCATION_GRAVE,0,1,1,e:GetHandler(),e,tp)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g,g:GetCount(),0,0)
end
function c76442347.spop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) and Duel.GetLocationCount(tp,LOCATION_MZONE)>0 then
		local spos=0
		if tc:IsCanBeSpecialSummoned(e,0,tp,false,false) then spos=spos+POS_FACEUP_ATTACK end
		if tc:IsCanBeSpecialSummoned(e,0,tp,false,false,POS_FACEDOWN) then spos=spos+POS_FACEDOWN_DEFENCE end
		if spos~=0 then Duel.SpecialSummon(tc,0,tp,tp,false,false,spos) end
	end
end
