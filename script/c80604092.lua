--血の代償(EU)
function c80604092.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c80604092.cost1)
	e1:SetTarget(c80604092.target1)
	e1:SetOperation(c80604092.activate1)
	c:RegisterEffect(e1)
	--instant
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(80604092,0))
	e3:SetCategory(CATEGORY_SUMMON)
	e3:SetType(EFFECT_TYPE_QUICK_O)
	e3:SetRange(LOCATION_SZONE)
	e3:SetCode(EVENT_FREE_CHAIN)
	e3:SetCondition(c80604092.condition2)
	e3:SetCost(c80604092.cost2)
	e3:SetTarget(c80604092.target2)
	e3:SetOperation(c80604092.activate2)
	c:RegisterEffect(e3)
end
function c80604092.filter(c)
	return c:IsSummonable(true,nil) or c:IsMSetable(true,nil)
end
function c80604092.cost1(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	e:SetLabel(0)
	local tn=Duel.GetTurnPlayer()
	local ph=Duel.GetCurrentPhase()
	if (tn==tp and ph~=PHASE_MAIN1 and ph~=PHASE_MAIN2) or (tn~=tp and ph~=PHASE_BATTLE) then return false end
	if Duel.GetLP(tp)<=500 then return end
	if Duel.IsExistingMatchingCard(c80604092.filter,tp,LOCATION_HAND+LOCATION_MZONE,0,1,nil)
		and Duel.SelectYesNo(tp,aux.Stringid(80604092,1)) then
		Duel.PayLPCost(tp,500)
		e:SetLabel(1)
	end
end
function c80604092.target1(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	if e:GetLabel()~=1 then return end
	Duel.SetOperationInfo(0,CATEGORY_SUMMON,nil,1,0,0)
end
function c80604092.activate1(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if not c:IsRelateToEffect(e) then return end
	if e:GetLabel()~=1 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SUMMON)
	local g=Duel.SelectMatchingCard(tp,c80604092.filter,tp,LOCATION_HAND+LOCATION_MZONE,0,1,1,nil)
	local tc=g:GetFirst()
	if tc then
		if tc:IsSummonable(true,nil) and (not tc:IsMSetable(true,nil) 
			or Duel.SelectPosition(tp,tc,POS_FACEUP_ATTACK+POS_FACEDOWN_DEFENCE)==POS_FACEUP_ATTACK) then
			Duel.Summon(tp,tc,true,nil)
		else Duel.MSet(tp,tc,true,nil) end
	end
end
function c80604092.condition2(e,tp,eg,ep,ev,re,r,rp)
	local tn=Duel.GetTurnPlayer()
	local ph=Duel.GetCurrentPhase()
	return (tn==tp and (ph==PHASE_MAIN1 or ph==PHASE_MAIN2)) or (tn~=tp and ph==PHASE_BATTLE)
end
function c80604092.cost2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckLPCost(tp,500)
	else Duel.PayLPCost(tp,500)	end
end
function c80604092.target2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		if not e:GetHandler():IsStatus(STATUS_CHAINING) then
			local ct=Duel.GetMatchingGroupCount(c80604092.filter,tp,LOCATION_HAND+LOCATION_MZONE,0,nil)
			e:SetLabel(ct)
			return ct>0
		else return e:GetLabel()>0 end
	end
	e:SetLabel(e:GetLabel()-1)
	Duel.SetOperationInfo(0,CATEGORY_SUMMON,nil,1,0,0)
end
function c80604092.activate2(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SUMMON)
	local g=Duel.SelectMatchingCard(tp,c80604092.filter,tp,LOCATION_HAND+LOCATION_MZONE,0,1,1,nil)
	local tc=g:GetFirst()
	if tc then
		local s1=tc:IsSummonable(true,nil)
		local s2=tc:IsMSetable(true,nil)
		if (s1 and s2 and Duel.SelectPosition(tp,tc,POS_FACEUP_ATTACK+POS_FACEDOWN_DEFENCE)==POS_FACEUP_ATTACK) or not s2 then
			Duel.Summon(tp,tc,true,nil)
		else
			Duel.MSet(tp,tc,true,nil)
		end
	end
end
