--先史遺産技術
function c90951921.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TOHAND+CATEGORY_SEARCH+CATEGORY_REMOVE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCountLimit(1,90951921+EFFECT_COUNT_CODE_OATH)
	e1:SetCost(c90951921.cost)
	e1:SetTarget(c90951921.target)
	e1:SetOperation(c90951921.activate)
	c:RegisterEffect(e1)
	if not c90951921.global_check then
		c90951921.global_check=true
		c90951921[0]=true
		c90951921[1]=true
		local ge1=Effect.CreateEffect(c)
		ge1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge1:SetCode(EVENT_SPSUMMON_SUCCESS)
		ge1:SetOperation(c90951921.checkop)
		Duel.RegisterEffect(ge1,0)
		local ge2=Effect.CreateEffect(c)
		ge2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge2:SetCode(EVENT_PHASE_START+PHASE_DRAW)
		ge2:SetOperation(c90951921.clear)
		Duel.RegisterEffect(ge2,0)
	end
end
function c90951921.checkop(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	while tc do
		if not tc:IsSetCard(0x70) then
			c90951921[tc:GetSummonPlayer()]=false
		end
		tc=eg:GetNext()
	end
end
function c90951921.clear(e,tp,eg,ep,ev,re,r,rp)
	c90951921[0]=true
	c90951921[1]=true
end
function c90951921.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return c90951921[tp] end
	--oath effects
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET+EFFECT_FLAG_OATH)
	e1:SetCode(EFFECT_CANNOT_SPECIAL_SUMMON)
	e1:SetReset(RESET_PHASE+PHASE_END)
	e1:SetTargetRange(1,0)
	e1:SetTarget(c90951921.splimit)
	Duel.RegisterEffect(e1,tp)
end
function c90951921.splimit(e,c,sump,sumtype,sumpos,targetp,se)
	return not c:IsSetCard(0x70)
end
function c90951921.filter(c)
	return c:IsType(TYPE_MONSTER) and c:IsSetCard(0x70) and c:IsAbleToRemove()
end
function c90951921.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c90951921.filter(chkc) end
	if chk==0 then
		if Duel.GetFieldGroupCount(tp,LOCATION_DECK,0)<3
			or not Duel.IsExistingTarget(c90951921.filter,tp,LOCATION_GRAVE,0,1,nil) then return false end
		local g=Duel.GetDecktopGroup(tp,2)
		return g:FilterCount(Card.IsAbleToHand,nil)>0
	end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g=Duel.SelectTarget(tp,c90951921.filter,tp,LOCATION_GRAVE,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,g,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,0,LOCATION_DECK)
end
function c90951921.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and Duel.Remove(tc,POS_FACEUP,REASON_EFFECT)~=0 then
		Duel.BreakEffect()
		local g=Duel.GetDecktopGroup(tp,2)
		Duel.ConfirmCards(tp,g)
		if g:GetCount()>0 then
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
			local sg=g:FilterSelect(tp,Card.IsAbleToHand,1,1,nil)
			g:Sub(sg)
			if sg:GetCount()>0 then
				Duel.DisableShuffleCheck()
				Duel.SendtoHand(sg,nil,REASON_EFFECT)
			end
			if g:GetCount()>0 then
				Duel.DisableShuffleCheck()
				Duel.SendtoGrave(g,REASON_EFFECT)
			end
		end
	end
end
