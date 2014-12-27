--音響戦士ピアーノ
function c31826057.initial_effect(c)
	--race change
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(31826057,0))
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetTarget(c31826057.target1)
	e1:SetOperation(c31826057.operation)
	c:RegisterEffect(e1)
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(31826057,0))
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_GRAVE)
	e2:SetCost(c31826057.cost)
	e2:SetTarget(c31826057.target2)
	e2:SetOperation(c31826057.operation)
	c:RegisterEffect(e2)
end
function c31826057.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToRemoveAsCost() end
	Duel.Remove(e:GetHandler(),POS_FACEUP,REASON_COST)
end
function c31826057.filter(c)
	return c:IsFaceup() and c:IsSetCard(0x1066) and c:IsLevelAbove(1)
end
function c31826057.target1(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and c31826057.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c31826057.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	Duel.SelectTarget(tp,c31826057.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,nil)
	Duel.Hint(HINT_SELECTMSG,tp,563)
	local rc=Duel.AnnounceRace(tp,1,0xffffff)
	e:SetLabel(rc)
end
function c31826057.target2(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(tp) and c31826057.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c31826057.filter,tp,LOCATION_MZONE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	Duel.SelectTarget(tp,c31826057.filter,tp,LOCATION_MZONE,0,1,1,nil)
	Duel.Hint(HINT_SELECTMSG,tp,563)
	local rc=Duel.AnnounceRace(tp,1,0xffffff)
	e:SetLabel(rc)
end
function c31826057.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=Duel.GetFirstTarget()
	if tc:IsFaceup() and tc:IsRelateToEffect(e) then
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_CHANGE_RACE)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		e1:SetValue(e:GetLabel())
		tc:RegisterEffect(e1)
	end
end
