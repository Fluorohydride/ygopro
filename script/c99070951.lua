--サイ・ガール
function c99070951.initial_effect(c)
	--remove
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(99070951,0))
	e1:SetCategory(CATEGORY_REMOVE)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_SPSUMMON_SUCCESS)
	e1:SetCondition(c99070951.rmcon)
	e1:SetTarget(c99070951.rmtg)
	e1:SetOperation(c99070951.rmop)
	c:RegisterEffect(e1)
	--add to hand
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(99070951,1))
	e2:SetCategory(CATEGORY_TOHAND)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e2:SetCode(EVENT_TO_GRAVE)
	e2:SetCondition(c99070951.thcon)
	e2:SetTarget(c99070951.thtg)
	e2:SetOperation(c99070951.thop)
	e2:SetLabelObject(e1)
	c:RegisterEffect(e2)
end
function c99070951.rmcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():IsPreviousLocation(LOCATION_REMOVED)
end
function c99070951.rmtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,nil,1,tp,LOCATION_DECK)
end
function c99070951.rmop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local g=Duel.GetDecktopGroup(tp,1)
	local tc=g:GetFirst()
	if not tc or not tc:IsAbleToRemove() then return end
	Duel.DisableShuffleCheck()
	Duel.Remove(tc,POS_FACEDOWN,REASON_EFFECT)
	tc:RegisterFlagEffect(99070951,RESET_EVENT+0x1fe0000,0,1)
	if c:IsLocation(LOCATION_MZONE) then
		c:RegisterFlagEffect(99070951,RESET_EVENT+0x680000,0,1)
	end
	e:SetLabelObject(tc)
end
function c99070951.thcon(e,tp,eg,ep,ev,re,r,rp)
	local tc=e:GetLabelObject():GetLabelObject()
	return e:GetHandler():IsPreviousLocation(LOCATION_ONFIELD) and e:GetHandler():GetFlagEffect(99070951)~=0
		and tc and tc:GetFlagEffect(99070951)~=0
end
function c99070951.thtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local tc=e:GetLabelObject():GetLabelObject()
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,tc,1,0,0)
end
function c99070951.thop(e,tp,eg,ep,ev,re,r,rp)
	local tc=e:GetLabelObject():GetLabelObject()
	if tc and tc:GetFlagEffect(99070951)~=0 then
		Duel.SendtoHand(tc,nil,REASON_EFFECT)
	end
end
