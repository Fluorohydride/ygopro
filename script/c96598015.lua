--金満な壺
function c96598015.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TODECK+CATEGORY_DRAW)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCountLimit(1,96598015+EFFECT_COUNT_CODE_OATH)
	e1:SetCost(c96598015.cost)
	e1:SetTarget(c96598015.target)
	e1:SetOperation(c96598015.activate)
	c:RegisterEffect(e1)
	if not c96598015.global_check then
		c96598015.global_check=true
		local ge1=Effect.CreateEffect(c)
		ge1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge1:SetCode(EVENT_SPSUMMON_SUCCESS)
		ge1:SetOperation(c96598015.checkop)
		Duel.RegisterEffect(ge1,0)
	end
end
function c96598015.checkop(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	local p1=false
	local p2=false
	while tc do
		if tc:GetSummonType()~=SUMMON_TYPE_PENDULUM then
			if tc:GetSummonPlayer()==0 then p1=true else p2=true end
		end
		tc=eg:GetNext()
	end
	if p1 then Duel.RegisterFlagEffect(0,96598015,RESET_PHASE+PHASE_END,0,1) end
	if p2 then Duel.RegisterFlagEffect(1,96598015,RESET_PHASE+PHASE_END,0,1) end
end
function c96598015.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFlagEffect(tp,96598015)==0 end
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET+EFFECT_FLAG_OATH)
	e1:SetCode(EFFECT_CANNOT_SPECIAL_SUMMON)
	e1:SetReset(RESET_PHASE+PHASE_END)
	e1:SetTargetRange(1,0)
	e1:SetLabelObject(e)
	e1:SetTarget(c96598015.splimit)
	Duel.RegisterEffect(e1,tp)
end
function c96598015.splimit(e,c,sump,sumtype,sumpos,targetp,se)
	return sumtype~=SUMMON_TYPE_PENDULUM and e:GetLabelObject()~=se
end
function c96598015.filter(c)
	return c:IsType(TYPE_PENDULUM) and c:IsAbleToDeck() and c:IsFaceup() and not c:IsHasEffect(EFFECT_NECRO_VALLEY)
end
function c96598015.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsPlayerCanDraw(tp,2)
		and Duel.IsExistingMatchingCard(c96598015.filter,tp,LOCATION_GRAVE+LOCATION_EXTRA,0,3,nil) end
	Duel.SetOperationInfo(0,CATEGORY_TODECK,nil,3,0,LOCATION_EXTRA+LOCATION_GRAVE)
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,2)
end
function c96598015.activate(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetMatchingGroupCount(c96598015.filter,tp,LOCATION_GRAVE+LOCATION_EXTRA,0,nil)<3 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TODECK)
	local tg=Duel.SelectMatchingCard(tp,c96598015.filter,tp,LOCATION_GRAVE+LOCATION_EXTRA,0,3,3,nil)
	Duel.SendtoDeck(tg,nil,0,REASON_EFFECT)
	local g=Duel.GetOperatedGroup()
	local ct=g:FilterCount(Card.IsLocation,nil,LOCATION_DECK)
	if ct==3 then
		Duel.ShuffleDeck(tp)
		Duel.BreakEffect()
		Duel.Draw(tp,2,REASON_EFFECT)
	end
end
