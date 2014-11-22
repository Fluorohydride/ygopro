--連成する振動
function c15582767.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c15582767.target1)
	e1:SetOperation(c15582767.operation)
	c:RegisterEffect(e1)
	--destroy
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(15582767,1))
	e2:SetCategory(CATEGORY_DESTROY+CATEGORY_DRAW)
	e2:SetType(EFFECT_TYPE_QUICK_O)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EVENT_FREE_CHAIN)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetCountLimit(1,15582767)
	e2:SetCost(c15582767.cost)
	e2:SetTarget(c15582767.target2)
	e2:SetOperation(c15582767.operation)
	e2:SetLabel(1)
	c:RegisterEffect(e2)
end
function c15582767.filter(c)
	return (c:GetSequence()==6 or c:GetSequence()==7) and c:IsDestructable()
end
function c15582767.target1(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_SZONE) and chkc:IsControler(tp) and c15582767.filter(chkc) end
	if chk==0 then return true end
	if Duel.GetFlagEffect(tp,15582767)==0 and Duel.IsPlayerCanDraw(tp,1)
		and Duel.IsExistingTarget(c15582767.filter,tp,LOCATION_SZONE,0,1,nil)
		and Duel.SelectYesNo(tp,aux.Stringid(15582767,0)) then
		e:SetCategory(CATEGORY_DESTROY+CATEGORY_DRAW)
		e:SetProperty(EFFECT_FLAG_CARD_TARGET)
		Duel.RegisterFlagEffect(tp,15582767,RESET_PHASE+PHASE_END,0,1)
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
		local g=Duel.SelectTarget(tp,c15582767.filter,tp,LOCATION_SZONE,0,1,1,nil)
		Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
		Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,1)
		e:SetLabel(1)
	else
		e:SetCategory(0)
		e:SetProperty(0)
		e:SetLabel(0)
	end
end
function c15582767.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFlagEffect(tp,15582767)==0 end
	Duel.RegisterFlagEffect(tp,15582767,RESET_PHASE+PHASE_END,0,1)
end
function c15582767.target2(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_SZONE) and chkc:IsControler(tp) and c15582767.filter(chkc) end
	if chk==0 then return Duel.IsPlayerCanDraw(tp,1)
		and Duel.IsExistingTarget(c15582767.filter,tp,LOCATION_SZONE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,c15582767.filter,tp,LOCATION_SZONE,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,1)
end
function c15582767.operation(e,tp,eg,ep,ev,re,r,rp)
	if e:GetLabel()==0 or not e:GetHandler():IsRelateToEffect(e) then return end
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and Duel.Destroy(tc,REASON_EFFECT)~=0 then
		Duel.BreakEffect()
		Duel.Draw(tp,1,REASON_EFFECT)
	end
end
