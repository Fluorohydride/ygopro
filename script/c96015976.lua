--再転
function c96015976.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c96015976.target1)
	e1:SetOperation(c96015976.operation)
	c:RegisterEffect(e1)
	--
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(96015976,0))
	e2:SetType(EFFECT_TYPE_QUICK_O)
	e2:SetRange(LOCATION_SZONE)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetCode(EVENT_FREE_CHAIN)
	e2:SetCost(c96015976.cost)
	e2:SetTarget(c96015976.target2)
	e2:SetOperation(c96015976.operation)
	c:RegisterEffect(e2)
end
function c96015976.target1(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and c96015976.filter(chkc) end
	if chk==0 then return true end
	if Duel.IsExistingTarget(c96015976.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil) and Duel.SelectYesNo(tp,94) then
		e:SetProperty(EFFECT_FLAG_CARD_TARGET)
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
		Duel.SelectTarget(tp,c96015976.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,nil)
		e:GetHandler():RegisterFlagEffect(96015976,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
	else
		e:SetProperty(0)
	end
end
function c96015976.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():GetFlagEffect(96015976)==0 end
	e:GetHandler():RegisterFlagEffect(96015976,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
end
function c96015976.filter(c)
	local lv=c:GetLevel()
	return c:IsFaceup() and lv~=0 and lv~=c:GetOriginalLevel()
end
function c96015976.target2(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and c96015976.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c96015976.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	Duel.SelectTarget(tp,c96015976.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,nil)
end
function c96015976.operation(e,tp,eg,ep,ev,re,r,rp)
	if e:GetHandler():GetFlagEffect(96015976)==0 then return end
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and tc:IsFaceup() then
		local dc=Duel.TossDice(tp,1)
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_CHANGE_LEVEL)
		e1:SetValue(dc)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		tc:RegisterEffect(e1)
	end
end
