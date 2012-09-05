--義賊の極意書
function c95096437.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_ATKCHANGE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c95096437.target)
	e1:SetOperation(c95096437.activate)
	c:RegisterEffect(e1)
end
function c95096437.filter(c)
	return c:IsFaceup() and c:IsType(TYPE_NORMAL)
end
function c95096437.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(tp) and c95096437.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c95096437.filter,tp,LOCATION_MZONE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	Duel.SelectTarget(tp,c95096437.filter,tp,LOCATION_MZONE,0,1,1,nil)
end
function c95096437.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and tc:IsFaceup() and not tc:IsImmuneToEffect(e) then
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetDescription(aux.Stringid(95096437,0))
		e1:SetCategory(CATEGORY_HANDES)
		e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
		e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
		e1:SetCode(EVENT_BATTLE_DAMAGE)
		e1:SetLabel(tc:GetFieldID())
		e1:SetCondition(c95096437.hdcon)
		e1:SetTarget(c95096437.hdtg)
		e1:SetOperation(c95096437.hdop)
		e1:SetReset(RESET_PHASE+PHASE_END)
		Duel.RegisterEffect(e1,tp)
	end
end
function c95096437.hdcon(e,tp,eg,ep,ev,re,r,rp)
	return ep~=tp and eg:GetFirst():GetFieldID()==e:GetLabel()
end
function c95096437.hdtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetTargetPlayer(tp)
	Duel.SetOperationInfo(0,CATEGORY_HANDES,nil,0,1-tp,2)
end
function c95096437.hdop(e,tp,eg,ep,ev,re,r,rp)
	local p=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER)
	local g=Duel.GetFieldGroup(p,0,LOCATION_HAND)
	local dg=g:RandomSelect(tp,2)
	Duel.SendtoGrave(dg,REASON_EFFECT+REASON_DISCARD)
end
