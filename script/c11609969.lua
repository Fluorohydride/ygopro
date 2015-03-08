--DD魔導賢者ケプラー
function c11609969.initial_effect(c)
	--pendulum summon
	aux.AddPendulumProcedure(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--splimit
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetRange(LOCATION_PZONE)
	e2:SetCode(EFFECT_CANNOT_SPECIAL_SUMMON)
	e2:SetProperty(EFFECT_FLAG_PLAYER_TARGET+EFFECT_FLAG_CANNOT_DISABLE)
	e2:SetTargetRange(1,0)
	e2:SetCondition(c11609969.splimcon)
	e2:SetTarget(c11609969.splimit)
	c:RegisterEffect(e2)
	--scale change
	local e3=Effect.CreateEffect(c)
	e3:SetCategory(CATEGORY_DESTROY)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e3:SetRange(LOCATION_PZONE)
	e3:SetCode(EVENT_PHASE+PHASE_STANDBY)
	e3:SetCountLimit(1)
	e3:SetCondition(c11609969.sccon)
	e3:SetTarget(c11609969.sctg)
	e3:SetOperation(c11609969.scop)
	c:RegisterEffect(e3)
	--tohand
	local e4=Effect.CreateEffect(c)
	e4:SetCategory(CATEGORY_TOHAND+CATEGORY_SEARCH)
	e4:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e4:SetCode(EVENT_SUMMON_SUCCESS)
	e4:SetProperty(EFFECT_FLAG_CARD_TARGET+EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_DELAY)
	e4:SetCountLimit(1,11609969)
	e4:SetTarget(c11609969.thtg)
	e4:SetOperation(c11609969.thop)
	c:RegisterEffect(e4)
	local e5=e4:Clone()
	e5:SetCode(EVENT_SPSUMMON_SUCCESS)
	c:RegisterEffect(e5)
end
function c11609969.splimcon(e)
	return not e:GetHandler():IsForbidden()
end
function c11609969.splimit(e,c,tp,sumtp,sumpos)
	return not c:IsSetCard(0xaf) and bit.band(sumtp,SUMMON_TYPE_PENDULUM)==SUMMON_TYPE_PENDULUM
end
function c11609969.sccon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()==tp
end
function c11609969.filter(c,lv)
	return c:IsFaceup() and not c:IsSetCard(0xaf) and c:IsLevelAbove(lv) and c:IsDestructable()
end
function c11609969.sctg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local scl=math.max(1,e:GetHandler():GetLeftScale()-2)
	local g=Duel.GetMatchingGroup(c11609969.filter,tp,LOCATION_MZONE,0,nil,scl)
	if e:GetHandler():GetLeftScale()>1 then
		Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
	end
end
function c11609969.scop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if not c:IsRelateToEffect(e) or c:GetLeftScale()==1 then return end
	local scl=2
	if c:GetLeftScale()==2 then scl=1 end
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_UPDATE_LSCALE)
	e1:SetValue(-scl)
	e1:SetReset(RESET_EVENT+0x1ff0000)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EFFECT_UPDATE_RSCALE)
	c:RegisterEffect(e2)
	local g=Duel.GetMatchingGroup(c11609969.filter,tp,LOCATION_MZONE,0,nil,scl)
	if g:GetCount()>0 then
		Duel.BreakEffect()
		Duel.Destroy(g,REASON_EFFECT)
	end
end
function c11609969.filter1(c)
	return c:IsFaceup() and c:IsSetCard(0xaf) and c:IsAbleToHand()
end
function c11609969.filter2(c)
	return c:IsSetCard(0xae) and c:IsAbleToHand()
end
function c11609969.thtg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsOnField() and c11609969.filter1(chkc) and chkc~=e:GetHandler() end
	local b1=Duel.IsExistingTarget(c11609969.filter1,tp,LOCATION_ONFIELD,0,1,e:GetHandler())
	local b2=Duel.IsExistingMatchingCard(c11609969.filter2,tp,LOCATION_DECK,0,1,nil)
	if chk==0 then return b1 or b2 end
	local op=0
	if b1 and b2 then op=Duel.SelectOption(tp,aux.Stringid(11609969,0),aux.Stringid(11609969,1))
	elseif b1 then op=Duel.SelectOption(tp,aux.Stringid(11609969,0))
	else op=Duel.SelectOption(tp,aux.Stringid(11609969,1))+1 end
	e:SetLabel(op)
	if op==0 then
		e:SetProperty(EFFECT_FLAG_CARD_TARGET+EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_DELAY)
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RTOHAND)
		local g=Duel.SelectTarget(tp,c11609969.filter1,tp,LOCATION_ONFIELD,0,1,1,e:GetHandler())
		Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,1,0,0)
	else
		e:SetProperty(EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_DELAY)
		Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
	end
end
function c11609969.thop(e,tp,eg,ep,ev,re,r,rp)
	if e:GetLabel()==0 then
		local tc=Duel.GetFirstTarget()
		if tc:IsRelateToEffect(e) then
			Duel.SendtoHand(tc,nil,REASON_EFFECT)
			Duel.ConfirmCards(1-tp,tc)
		end
	else
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
		local g=Duel.SelectMatchingCard(tp,c11609969.filter2,tp,LOCATION_DECK,0,1,1,nil)
		if g:GetCount()>0 then
			Duel.SendtoHand(g,nil,REASON_EFFECT)
			Duel.ConfirmCards(1-tp,g)
		end
	end
end
