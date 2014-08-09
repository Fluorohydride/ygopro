--ＤＤ魔導賢者ケプラー
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
	e2:SetTarget(c11609969.splimit)
	c:RegisterEffect(e2)
	--change scale
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(11609969,0))
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e3:SetProperty(EFFECT_FLAG_REPEAT)
	e3:SetCategory(CATEGORY_DESTROY)
	e3:SetCode(EVENT_PHASE+PHASE_STANDBY)
	e3:SetRange(LOCATION_PZONE)
	e3:SetCountLimit(1)
	e3:SetCondition(c11609969.scalecon)
	e3:SetTarget(c11609969.scaletg)
	e3:SetOperation(c11609969.scaleop)
	c:RegisterEffect(e3)
	--to hand
	local e4=Effect.CreateEffect(c)
	e4:SetDescription(aux.Stringid(11609969,1))
	e4:SetCategory(CATEGORY_TOHAND)
	e4:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e4:SetProperty(EFFECT_FLAG_DAMAGE_STEP)
	e4:SetCode(EVENT_SUMMON_SUCCESS)
	e4:SetCountLimit(1,11609969)
	e4:SetTarget(c11609969.target)
	e4:SetOperation(c11609969.operation)
	c:RegisterEffect(e4)
	--
	local e5=e4:Clone()
	e5:SetCode(EVENT_SPSUMMON_SUCCESS)
	c:RegisterEffect(e5)
	--search
	local e6=Effect.CreateEffect(c)
	e6:SetDescription(aux.Stringid(11609969,2))
	e6:SetCategory(CATEGORY_TOHAND+CATEGORY_SEARCH)
	e6:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e6:SetProperty(EFFECT_FLAG_DAMAGE_STEP)
	e6:SetCode(EVENT_SUMMON_SUCCESS)
	e6:SetCountLimit(1,11609969)
	e6:SetTarget(c11609969.shtg)
	e6:SetOperation(c11609969.shop)
	c:RegisterEffect(e6)
	local e7=e6:Clone()
	e7:SetCode(EVENT_SPSUMMON_SUCCESS)
	c:RegisterEffect(e7)
end
function c11609969.con(e,tp,eg,ep,ev,re,r,rp)
	local seq=e:GetHandler():GetSequence()
	return seq==6 or seq==7
end
function c11609969.splimit(e,c,sump,sumtype,sumpos,targetp,se)
	return sumtype==SUMMON_TYPE_PENDULUM and not c:IsSetCard(0xaf)
end
function c11609969.scalecon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()==tp
end
function c11609969.scaletg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	local c=e:GetHandler()
	if chk==0 then return true end
	local dg=Duel.GetMatchingGroup(c11609969.dfilter,tp,LOCATION_MZONE,0,nil,c)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,dg,dg:GetCount(),0,0)
end
function c11609969.dfilter(c,rc)
	local lscale=rc:GetLeftScale()
	local rscale=rc:GetRightScale()
	return c:IsFaceup() and c:IsDestructable() and not c:IsType(TYPE_XYZ) and (c:GetLevel()>lscale or c:GetLevel()>rscale) and not c:IsSetCard(0xaf)
end
function c11609969.scaleop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local lscale=c:GetLeftScale()
	local rscale=c:GetRightScale()
	if c:IsFacedown() or not c:IsRelateToEffect(e) then return end
	if lscale>1 or rscale>1 then
		
		local val=2
		if lscale==2 then val=1 end
	
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_UPDATE_LSCALE)
		e1:SetValue(-val)
		e1:SetReset(RESET_EVENT+0x1ff0000)
		c:RegisterEffect(e1)
		if rscale==2 then val=1  else val=2 end
		local e2=e1:Clone()
		e2:SetCode(EFFECT_UPDATE_RSCALE)
		e2:SetValue(-val)
		c:RegisterEffect(e2)
		
		Duel.BreakEffect()
		local dg=Duel.GetMatchingGroup(c11609969.dfilter,tp,LOCATION_MZONE,0,nil,c)
		if dg:GetCount()>0 then
			Duel.Destroy(dg,REASON_EFFECT)
		end
	end
end
function c11609969.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToGraveAsCost() end
	Duel.SendtoGrave(e:GetHandler(),REASON_COST)
end
function c11609969.thfilter(c,rc)
	return c:IsFaceup() and c:IsSetCard(0xaf) and c:IsAbleToHand()
end
function c11609969.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_ONFIELD) and chkc:IsControler(tp) and c11609969.thfilter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c11609969.thfilter,tp,LOCATION_ONFIELD,0,1,e:GetHandler()) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RTOHAND)
	local g=Duel.SelectTarget(tp,c11609969.thfilter,tp,LOCATION_ONFIELD,0,1,1,e:GetHandler())
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,1,0,0)
end
function c11609969.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) then
		Duel.SendtoHand(tc,nil,REASON_EFFECT)
	end
end
function c11609969.filter(c)
	return c:IsSetCard(0xae) and c:IsAbleToHand()
end
function c11609969.shtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c11609969.filter,tp,LOCATION_DECK,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
end
function c11609969.shop(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectMatchingCard(tp,c11609969.filter,tp,LOCATION_DECK,0,1,1,nil)
	if g:GetCount()>0 then
		Duel.SendtoHand(g,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,g)
	end
end