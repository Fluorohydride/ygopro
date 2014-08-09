--ＤＤ魔導賢者ガリレイ
function c74605254.initial_effect(c)
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
	e2:SetTarget(c74605254.splimit)
	c:RegisterEffect(e2)
	--change scale
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(74605254,0))
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e3:SetProperty(EFFECT_FLAG_REPEAT)
	e3:SetCategory(CATEGORY_DESTROY)
	e3:SetCode(EVENT_PHASE+PHASE_STANDBY)
	e3:SetRange(LOCATION_PZONE)
	e3:SetCountLimit(1)
	e3:SetCondition(c74605254.scalecon)
	e3:SetTarget(c74605254.scaletg)
	e3:SetOperation(c74605254.scaleop)
	c:RegisterEffect(e3)
	--to hand
	local e4=Effect.CreateEffect(c)
	e4:SetDescription(aux.Stringid(74605254,1))
	e4:SetCategory(CATEGORY_TOHAND)
	e4:SetType(EFFECT_TYPE_QUICK_O)
	e4:SetCode(EVENT_FREE_CHAIN)
	e4:SetHintTiming(0,0x1e0)
	e4:SetRange(LOCATION_HAND)
	e4:SetCountLimit(1,74605254)
	e4:SetCost(c74605254.cost)
	e4:SetTarget(c74605254.target)
	e4:SetOperation(c74605254.operation)
	c:RegisterEffect(e4)
end
function c74605254.splimit(e,c,sump,sumtype,sumpos,targetp,se)
	return sumtype==SUMMON_TYPE_PENDULUM and not c:IsSetCard(0xaf)
end
function c74605254.scalecon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()==tp
end
function c74605254.scaletg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	local c=e:GetHandler()
	if chk==0 then return true end
	local dg=Duel.GetMatchingGroup(c74605254.dfilter,tp,LOCATION_MZONE,0,nil,c)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,dg,dg:GetCount(),0,0)
end
function c74605254.dfilter(c,rc)
	local lscale=rc:GetLeftScale()
	local rscale=rc:GetRightScale()
	return c:IsFaceup() and c:IsDestructable() and not c:IsType(TYPE_XYZ) and (c:GetLevel()<lscale or c:GetLevel()<rscale) and not c:IsSetCard(0xaf)
end
function c74605254.scaleop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local lscale=c:GetLeftScale()
	local rscale=c:GetRightScale()
	if c:IsFacedown() or not c:IsRelateToEffect(e) then return end
	if lscale<10 or rscale<10 then
		
		local val=2
		if lscale==9 then val=1 end
	
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_UPDATE_LSCALE)
		e1:SetValue(val)
		e1:SetReset(RESET_EVENT+0x1ff0000)
		c:RegisterEffect(e1)
		if rscale==9 then val=1  else val=2 end
		local e2=e1:Clone()
		e2:SetCode(EFFECT_UPDATE_RSCALE)
		e2:SetValue(val)
		c:RegisterEffect(e2)
		
		Duel.BreakEffect()
		local dg=Duel.GetMatchingGroup(c74605254.dfilter,tp,LOCATION_MZONE,0,nil,c)
		if dg:GetCount()>0 then
			Duel.Destroy(dg,REASON_EFFECT)
		end
	end
end
function c74605254.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToGraveAsCost() end
	Duel.SendtoGrave(e:GetHandler(),REASON_COST)
end
function c74605254.thfilter(c,rc)
	return c:IsFaceup() and (c:IsSetCard(0xae) or c:IsSetCard(0xaf)) and c:IsAbleToHand()
end
function c74605254.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_ONFIELD) and chkc:IsControler(tp) and c74605254.thfilter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c74605254.thfilter,tp,LOCATION_ONFIELD,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RTOHAND)
	local g=Duel.SelectTarget(tp,c74605254.thfilter,tp,LOCATION_ONFIELD,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,1,0,0)
end
function c74605254.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) then
		Duel.SendtoHand(tc,nil,REASON_EFFECT)
	end
end