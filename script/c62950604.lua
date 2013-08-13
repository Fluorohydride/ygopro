--沈黙のサイコウィザード
function c62950604.initial_effect(c)
	--remove
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(62950604,0))
	e1:SetCategory(CATEGORY_REMOVE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_SUMMON_SUCCESS)
	e1:SetTarget(c62950604.rmtg)
	e1:SetOperation(c62950604.rmop)
	c:RegisterEffect(e1)
	--special summon
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(62950604,1))
	e2:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e2:SetCode(EVENT_TO_GRAVE)
	e2:SetCondition(c62950604.spcon)
	e2:SetTarget(c62950604.sptg)
	e2:SetOperation(c62950604.spop)
	c:RegisterEffect(e2)
	e1:SetLabelObject(e2)
end
function c62950604.filter(c)
	return c:IsRace(RACE_PSYCHO) and c:IsAbleToRemove()
end
function c62950604.rmtg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c62950604.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c62950604.filter,tp,LOCATION_GRAVE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g=Duel.SelectTarget(tp,c62950604.filter,tp,LOCATION_GRAVE,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,g,1,0,0)
end
function c62950604.rmop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	local c=e:GetHandler()
	if tc:IsRelateToEffect(e) and Duel.Remove(tc,POS_FACEUP,REASON_EFFECT)~=0 and c:IsRelateToEffect(e) then
		c:RegisterFlagEffect(62950604,RESET_EVENT+0x1680000,0,0)
		tc:RegisterFlagEffect(62950604,RESET_EVENT+0x1fe0000,0,0)
		e:GetLabelObject():SetLabelObject(tc)
		e:GetLabelObject():SetLabel(1)
	end
end
function c62950604.spcon(e,tp,eg,ep,ev,re,r,rp)
	local tc=e:GetLabelObject()
	local act=e:GetLabel()
	local c=e:GetHandler()
	e:SetLabel(0)
	return tc and act==1 and e:GetHandler():IsPreviousLocation(LOCATION_ONFIELD)
		and c:GetFlagEffect(62950604)~=0
end
function c62950604.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	local tc=e:GetLabelObject()
	if chk==0 then return tc:GetFlagEffect(62950604)~=0 end
	tc:CreateEffectRelation(e)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,tc,1,0,0)
end
function c62950604.spop(e,tp,eg,ep,ev,re,r,rp)
	local tc=e:GetLabelObject()
	if tc:IsRelateToEffect(e) then
		Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP)
	end
end
