--エルシャドール・ミドラーシュ
function c94977269.initial_effect(c)
	c:EnableReviveLimit()
	--fusion material
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCode(EFFECT_FUSION_MATERIAL)
	e1:SetCondition(c94977269.fuscon)
	e1:SetOperation(c94977269.fusop)
	c:RegisterEffect(e1)
	--splimit
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetProperty(EFFECT_FLAG_SINGLE_RANGE+EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e2:SetCode(EFFECT_SPSUMMON_CONDITION)
	e2:SetRange(LOCATION_EXTRA)
	e2:SetValue(c94977269.splimit)
	c:RegisterEffect(e2)
	--indes
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e3:SetCode(EFFECT_INDESTRUCTABLE_EFFECT)
	e3:SetRange(LOCATION_MZONE)
	e3:SetValue(c94977269.indval)
	c:RegisterEffect(e3)
	--disable summon
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_FIELD)
	e4:SetCode(EFFECT_SPSUMMON_COUNT_LIMIT)
	e4:SetRange(LOCATION_MZONE)
	e4:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e4:SetTargetRange(1,1)
	e4:SetValue(c94977269.spval)
	c:RegisterEffect(e4)
	local e5=Effect.CreateEffect(c)
	e5:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e5:SetCode(EVENT_SPSUMMON)
	e5:SetRange(LOCATION_MZONE)
	e5:SetOperation(c94977269.checkop1)
	c:RegisterEffect(e5)
	local e6=Effect.CreateEffect(c)
	e6:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e6:SetCode(EVENT_CHAINING)
	e6:SetRange(LOCATION_MZONE)
	e6:SetOperation(c94977269.checkop2)
	c:RegisterEffect(e6)
	local e7=Effect.CreateEffect(c)
	e7:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e7:SetCode(EVENT_CHAIN_SOLVING)
	e7:SetRange(LOCATION_MZONE)
	e7:SetOperation(c94977269.checkop3)
	c:RegisterEffect(e7)
	--tohand
	local e8=Effect.CreateEffect(c)
	e8:SetDescription(aux.Stringid(94977269,0))
	e8:SetCategory(CATEGORY_TOHAND)
	e8:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e8:SetCode(EVENT_TO_GRAVE)
	e8:SetProperty(EFFECT_FLAG_CARD_TARGET+EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_DELAY)
	e8:SetCondition(c94977269.thcon)
	e8:SetTarget(c94977269.thtg)
	e8:SetOperation(c94977269.thop)
	c:RegisterEffect(e8)
end
function c94977269.ffilter1(c)
	return c:IsSetCard(0x9d)
end
function c94977269.ffilter2(c)
	return c:IsAttribute(ATTRIBUTE_DARK) or c:IsHasEffect(4904633)
end
function c94977269.exfilter(c,g)
	return c:IsFaceup() and c:IsCanBeFusionMaterial() and not g:IsContains(c)
end
function c94977269.fuscon(e,g,gc,chkf)
	if g==nil then return true end
	local tp=e:GetHandlerPlayer()
	local fc=Duel.GetFieldCard(tp,LOCATION_SZONE,5)
	local exg=Group.CreateGroup()
	if fc and fc:IsHasEffect(81788994) and fc:IsCanRemoveCounter(tp,0x16,3,REASON_EFFECT) then
		local sg=Duel.GetMatchingGroup(c94977269.exfilter,tp,0,LOCATION_MZONE,nil,g)
		exg:Merge(sg)
	end
	if gc then return (c94977269.ffilter1(gc) and (g:IsExists(c94977269.ffilter2,1,gc) or exg:IsExists(c94977269.ffilter2,1,gc)))
		or (c94977269.ffilter2(gc) and (g:IsExists(c94977269.ffilter1,1,gc) or exg:IsExists(c94977269.ffilter1,1,gc))) end
	local g1=Group.CreateGroup()
	local g2=Group.CreateGroup()
	local g3=Group.CreateGroup()
	local g4=Group.CreateGroup()
	local tc=g:GetFirst()
	while tc do
		if c94977269.ffilter1(tc) then
			g1:AddCard(tc)
			if aux.FConditionCheckF(tc,chkf) then g3:AddCard(tc) end
		end
		if c94977269.ffilter2(tc) then
			g2:AddCard(tc)
			if aux.FConditionCheckF(tc,chkf) then g4:AddCard(tc) end
		end
		tc=g:GetNext()
	end
	local exg1=exg:Filter(c94977269.ffilter1,nil)
	local exg2=exg:Filter(c94977269.ffilter2,nil)
	if chkf~=PLAYER_NONE then
		return (g3:IsExists(aux.FConditionFilterF2,1,nil,g2)
			or g3:IsExists(aux.FConditionFilterF2,1,nil,exg2)
			or g4:IsExists(aux.FConditionFilterF2,1,nil,g1)
			or g4:IsExists(aux.FConditionFilterF2,1,nil,exg1))
	else
		return (g1:IsExists(aux.FConditionFilterF2,1,nil,g2)
			or g1:IsExists(aux.FConditionFilterF2,1,nil,exg2)
			or g2:IsExists(aux.FConditionFilterF2,1,nil,exg1))
	end
end
function c94977269.fusop(e,tp,eg,ep,ev,re,r,rp,gc,chkf)
	local fc=Duel.GetFieldCard(tp,LOCATION_SZONE,5)
	local exg=Group.CreateGroup()
	if fc and fc:IsHasEffect(81788994) and fc:IsCanRemoveCounter(tp,0x16,3,REASON_EFFECT) then
		local sg=Duel.GetMatchingGroup(c94977269.exfilter,tp,0,LOCATION_MZONE,nil,eg)
		exg:Merge(sg)
	end
	if gc then
		local sg1=Group.CreateGroup()
		local sg2=Group.CreateGroup()
		if c94977269.ffilter1(gc) then
			sg1:Merge(eg:Filter(c94977269.ffilter2,gc))
			sg2:Merge(exg:Filter(c94977269.ffilter2,gc))
		end
		if c94977269.ffilter2(gc) then
			sg1:Merge(eg:Filter(c94977269.ffilter1,gc))
			sg2:Merge(exg:Filter(c94977269.ffilter1,gc))
		end
		local g1=nil
		if sg1:GetCount()==0 or (sg2:GetCount()>0 and Duel.SelectYesNo(tp,aux.Stringid(81788994,0))) then
			fc:RemoveCounter(tp,0x16,3,REASON_EFFECT)
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
			g1=sg2:Select(tp,1,1,nil)
		else
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
			g1=sg1:Select(tp,1,1,nil)
		end
		Duel.SetFusionMaterial(g1)
		return
	end
	local sg=eg:Filter(aux.FConditionFilterF2c,nil,c94977269.ffilter1,c94977269.ffilter2)
	local g1=nil
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
	if chkf~=PLAYER_NONE then
		g1=sg:FilterSelect(tp,aux.FConditionCheckF,1,1,nil,chkf)
	else g1=sg:Select(tp,1,1,nil) end
	local tc1=g1:GetFirst()
	local sg1=Group.CreateGroup()
	local sg2=Group.CreateGroup()
	if c94977269.ffilter1(tc1) then
		sg1:Merge(sg:Filter(c94977269.ffilter2,tc1))
		sg2:Merge(exg:Filter(c94977269.ffilter2,tc1))
	end
	if c94977269.ffilter2(tc1) then
		sg1:Merge(sg:Filter(c94977269.ffilter1,tc1))
		sg2:Merge(exg:Filter(c94977269.ffilter1,tc1))
	end
	local g2=nil
	if sg1:GetCount()==0 or (sg2:GetCount()>0 and Duel.SelectYesNo(tp,aux.Stringid(81788994,0))) then
		fc:RemoveCounter(tp,0x16,3,REASON_EFFECT)
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
		g2=sg2:Select(tp,1,1,nil)
	else
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
		g2=sg1:Select(tp,1,1,nil)
	end
	g1:Merge(g2)
	Duel.SetFusionMaterial(g1)
end
function c94977269.splimit(e,se,sp,st)
	return bit.band(st,SUMMON_TYPE_FUSION)==SUMMON_TYPE_FUSION
end
function c94977269.spval(e,se,sp)
	return 1-e:GetHandler():GetFlagEffect(94977269+sp)
end
function c94977269.checkop1(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if eg:IsContains(c) then return end
	local p1=false
	local p2=false
	local tc=eg:GetFirst()
	while tc do
		if tc:GetSummonPlayer()==0 then p1=true else p2=true end
		tc=eg:GetNext()
	end
	if p1 then c:RegisterFlagEffect(94977269,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1) end
	if p2 then c:RegisterFlagEffect(94977270,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1) end
end
function c94977269.checkop2(e,tp,eg,ep,ev,re,r,rp)
	local ex,tg,ct,sp=Duel.GetOperationInfo(ev,CATEGORY_SPECIAL_SUMMON)
	local c=e:GetHandler()
	local p=0
	if ex then
		if sp==PLAYER_ALL then
			c:RegisterFlagEffect(94977269,RESET_EVENT+0x1fe0000+RESET_CHAIN,0,1)
			c:RegisterFlagEffect(94977270,RESET_EVENT+0x1fe0000+RESET_CHAIN,0,1)
			p=PLAYER_ALL
		else
			c:RegisterFlagEffect(94977269+rp,RESET_EVENT+0x1fe0000+RESET_CHAIN,0,1)
			p=rp
		end
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
		e1:SetCode(EVENT_CHAIN_ACTIVATING)
		e1:SetRange(LOCATION_MZONE)
		e1:SetLabel(p)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_CHAIN)
		e1:SetOperation(c94977269.rst)
		c:RegisterEffect(e1)
	end
end
function c94977269.rst(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local p=e:GetLabel()
	if p==PLAYER_ALL then
		c:ResetFlagEffect(94977269) 
		c:ResetFlagEffect(94977270) 
	else
		c:ResetFlagEffect(94977269+p)
	end
	e:Reset()
end
function c94977269.checkop3(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local ex,tg,ct,sp=Duel.GetOperationInfo(ev,CATEGORY_SPECIAL_SUMMON)
	if ex then
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
		e1:SetCode(EVENT_CHAIN_SOLVED)
		e1:SetRange(LOCATION_MZONE)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_CHAIN)
		e1:SetOperation(c94977269.checkop4)
		c:RegisterEffect(e1)
	end
end
function c94977269.checkop4(e,tp,eg,ep,ev,re,r,rp)
	local ex,tg,ct,sp=Duel.GetOperationInfo(ev,CATEGORY_SPECIAL_SUMMON)
	local c=e:GetHandler()
	if sp==PLAYER_ALL then
		c:RegisterFlagEffect(94977269,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
		c:RegisterFlagEffect(94977270,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
	else
		c:RegisterFlagEffect(94977269+rp,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
	end
	e:Reset()
end
function c94977269.indval(e,re,tp)
	return tp~=e:GetHandlerPlayer()
end
function c94977269.thcon(e,tp,eg,ep,ev,re,r,rp)
	return not e:GetHandler():IsReason(REASON_RETURN)
end
function c94977269.thfilter(c)
	return c:IsSetCard(0x9d) and c:IsType(TYPE_SPELL+TYPE_TRAP) and c:IsAbleToHand()
end
function c94977269.thtg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c94977269.thfilter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c94977269.thfilter,tp,LOCATION_GRAVE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectTarget(tp,c94977269.thfilter,tp,LOCATION_GRAVE,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,1,0,0)
end
function c94977269.thop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.SendtoHand(tc,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,tc)
	end
end
