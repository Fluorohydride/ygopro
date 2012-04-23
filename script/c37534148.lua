--リボーンリボン
function c37534148.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_EQUIP)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetTarget(c37534148.target)
	e1:SetOperation(c37534148.operation)
	c:RegisterEffect(e1)
	--Equip limit
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_EQUIP_LIMIT)
	e2:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e2:SetValue(c37534148.eqlimit)
	c:RegisterEffect(e2)
	--special summon
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(37534148,0))
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e3:SetRange(LOCATION_GRAVE)
	e3:SetCode(EVENT_BATTLE_DESTROYED)
	e3:SetCondition(c37534148.regcon)
	e3:SetOperation(c37534148.regop)
	c:RegisterEffect(e3)
end
function c37534148.eqlimit(e,c)
	return not c:IsHasEffect(EFFECT_CANNOT_SPECIAL_SUMMON)
end
function c37534148.filter(c)
	return c:IsFaceup() and not c:IsHasEffect(EFFECT_CANNOT_SPECIAL_SUMMON)
end
function c37534148.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and c37534148.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c37534148.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_EQUIP)
	Duel.SelectTarget(tp,c37534148.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_EQUIP,e:GetHandler(),1,0,0)
end
function c37534148.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=Duel.GetFirstTarget()
	if c:IsRelateToEffect(e) and tc:IsRelateToEffect(e) and tc:IsFaceup() then
		Duel.Equip(tp,c,tc)
	end
end
function c37534148.regcon(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	e:SetLabelObject(tc)
	return e:GetHandler():GetPreviousEquipTarget()==tc and tc:IsLocation(LOCATION_GRAVE) and tc:IsReason(REASON_BATTLE)
end
function c37534148.regop(e,tp,eg,ep,ev,re,r,rp)
	local tc=e:GetLabelObject()
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetDescription(aux.Stringid(37534148,1))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_PHASE+PHASE_END)
	e1:SetRange(LOCATION_GRAVE)
	e1:SetCountLimit(1)
	e1:SetTarget(c37534148.sptg)
	e1:SetOperation(c37534148.spop)
	e1:SetLabelObject(tc)
	e1:SetReset(RESET_EVENT+0x16c0000+RESET_PHASE+PHASE_END)
	e:GetHandler():RegisterEffect(e1)
	tc:RegisterFlagEffect(37534148,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
end
function c37534148.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetLabelObject():GetFlagEffect(37534148)~=0 end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetLabelObject(),1,0,0)
end
function c37534148.spop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	if e:GetLabelObject():GetFlagEffect(37534148)~=0 then
		Duel.SpecialSummon(e:GetLabelObject(),0,tp,tp,false,false,POS_FACEUP)
	end
end
