--CX 冀望皇バリアン
function c67926903.initial_effect(c)
	--xyz summon
	aux.AddXyzProcedure(c,nil,7,3,c67926903.ovfilter,aux.Stringid(67926903,0),5)
	c:EnableReviveLimit()
	--atk
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetValue(c67926903.atkval)
	c:RegisterEffect(e1)
	--copy
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(67926903,1))
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_MZONE)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetCountLimit(1,67926903)
	e2:SetTarget(c67926903.copytg)
	e2:SetOperation(c67926903.copyop)
	c:RegisterEffect(e2)
end
function c67926903.ovfilter(c)
	local code=c:GetCode()
	local class=_G["c"..code]
	if class==nil then return false end
	local no=class.xyz_number
	return c:IsFaceup() and no and no>=101 and no<=107 and c:IsSetCard(0x1048)
end
function c67926903.atkval(e,c)
	return c:GetOverlayCount()*1000
end
function c67926903.filter(c)
	return c:IsSetCard(0x48) and c:IsType(TYPE_EFFECT)
end
function c67926903.copytg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(tp) and chkc:IsLocation(LOCATION_GRAVE) and c67926903.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c67926903.filter,tp,LOCATION_GRAVE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TARGET)
	Duel.SelectTarget(tp,c67926903.filter,tp,LOCATION_GRAVE,0,1,1,nil)
end
function c67926903.copyop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=Duel.GetFirstTarget()
	if c:IsRelateToEffect(e) and c:IsFaceup() and tc:IsRelateToEffect(e) then
		local code=tc:GetOriginalCode()
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_CHANGE_CODE)
		e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
		e1:SetValue(code)
		e1:SetLabel(tp)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+RESET_END+RESET_OPPO_TURN)
		c:RegisterEffect(e1)
		local cid=c:CopyEffect(code,RESET_EVENT+0x1fe0000+RESET_PHASE+RESET_END+RESET_OPPO_TURN)
		local e2=Effect.CreateEffect(c)
		e2:SetDescription(aux.Stringid(67926903,2))
		e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		e2:SetCode(EVENT_PHASE+PHASE_END)
		e2:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
		e2:SetCountLimit(1)
		e2:SetRange(LOCATION_MZONE)
		e2:SetCondition(c67926903.rstcon)
		e2:SetOperation(c67926903.rstop)
		e2:SetLabel(cid)
		e2:SetLabelObject(e1)
		e2:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+RESET_END+RESET_OPPO_TURN)
		c:RegisterEffect(e2)
	end
end
function c67926903.rstcon(e,tp,eg,ep,ev,re,r,rp)
	local e1=e:GetLabelObject()
	return Duel.GetTurnPlayer()~=e1:GetLabel()
end
function c67926903.rstop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local cid=e:GetLabel()
	c:ResetEffect(cid,RESET_COPY)
	local e1=e:GetLabelObject()
	e1:Reset()
	Duel.HintSelection(Group.FromCards(c))
	Duel.Hint(HINT_OPSELECTED,1-tp,e:GetDescription())
end
